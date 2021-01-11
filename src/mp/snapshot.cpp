/*
 * Copyright (c) 2020 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include "snapshot.h"
#include "delta.h"
#include "../game/object/area.h"
#include "../common/log.h"

using namespace std;

using namespace reone::game;

namespace reone {

namespace mp {

void Snapshot::reset(uint32_t newSeq, uint32_t newTick) {
    objectAddition.clear();
    objectDeletion.clear();
    objectStat.clear();
    inventoryAddition.clear();
    inventoryDeletion.clear();
    animCommands.clear();
    bullets.clear();

    seq = newSeq;
    tick = newTick;
}

void History::freeze(uint32_t newTick) {
    ++_seq;

    if (_snapshots.size() < _maxLen) {
        _snapshots.push_back(make_unique<Snapshot>(_seq, newTick)); // TODO: add global tick!
        ++_top;
        return;
    }

    _top = (_top + 1) % _maxLen;
    unique_ptr<Snapshot>& toDelete = _snapshots[_top];

    for (auto &pr : toDelete->objectStat) {
        auto &seqList = _activeObjects[pr.first];
        while (seqList.back() <= toDelete->seq) {
            seqList.pop_back();
        }
        if (seqList.empty()) {
            _activeObjects.erase(pr.first);
        }

        _staleObjects[pr.first] = std::move(pr.second);
        pr.second = nullptr;
    }

    _snapshots[_top]->reset(_seq, 0); // TODO: add global tick!
}

void History::addObject(uint32_t id, std::string resRef) {
    getCurrent().objectAddition[id] = resRef;

    if (_activeObjects.count(id) != 0) {
        _activeObjects[id] = {};
    }

    _activeObjects[id].push_front(_seq);
}

void History::delObject(uint32_t id) {
    getCurrent().objectDeletion.push_back(id);

    if (_activeObjects.count(id) > 0) {
        _activeObjects.erase(id);
    }

    if (_staleObjects.count(id) > 0) {
        _staleObjects.erase(id);
    }
}

void History::updateObject(uint32_t id) {
    shared_ptr<SpatialObject> obj = _area->find(id);

    if (!obj) {
        debug("Object id not found: " + to_string(id));
    }

    getCurrent().objectStat[id] = obj->captureStatus();
    if (_activeObjects.count(id) != 0) {
        _activeObjects[id] = {};
    }
    _activeObjects[id].push_front(_seq); // newer to the front
}

void History::addToInventory(std::string resRef, uint16_t quantity) {
    getCurrent().inventoryAddition[resRef] = quantity; // TODO: +=
}

void History::remFrInventory(std::string resRef, uint16_t quantity) {
    getCurrent().inventoryDeletion[resRef] = quantity; // TODO: +=
}

void History::addBallistic(std::unique_ptr<Ballistic>&& bullet) {
    getCurrent().bullets.push_back(bullet);
}

void History::collectAnimCmds() {
    for (shared_ptr<SpatialObject> obj : _area->getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(obj);
        if (creature->_animInfo) {
            creature->_animInfo->timestamp = getCurrent().tick; // TODO: remove
            getCurrent().animCommands[creature->id()] = move(creature->_animInfo);
            creature->_animInfo = nullptr;
        }
    }
}

void History::serialize(StreamWriter &writer, uint32_t lastAckSeq) {
    // disconnect/lost-packet, resend state of all objects
    if (lastAckSeq + _maxLen <= _seq) {
        writer << _seq << uint32_t(0) /*dummy*/ << getCurrent().tick;
        serialize(writer);
        return;
    }

    // write packet header
    writer << _seq << lastAckSeq << getCurrent().tick;

    // list 1: object addition list
    map<uint32_t, string> totalAddition;
    for (uint32_t s = lastAckSeq; s < _seq; ++s) {
        auto &snapshot = getSnapshot(s);
        totalAddition.insert(
            snapshot.objectAddition.begin(),
            snapshot.objectAddition.end());
    }
    writer << totalAddition.size();
    for (const auto &pr : totalAddition) {
        writer << uint32_t(pr.first) << pr.second;
    }
    totalAddition.clear(); // save memory

    // list 2: object deletion list
    list<uint32_t> totalDeletion;
    for (uint32_t s = lastAckSeq; s < _seq; ++s) {
        auto &snapshot = getSnapshot(s);
        totalDeletion.insert(totalDeletion.end(),
            snapshot.objectDeletion.begin(),
            snapshot.objectDeletion.end());
    }
    writer << totalDeletion.size();
    for (uint32_t id : totalDeletion) {
        writer << id;
    }
    totalDeletion.clear();

    // list 3: object delta strings
    list<uint32_t> changedObjs;
    for (const auto& pr : _activeObjects) {
        uint32_t latestSeq = pr.second.front();
        if (latestSeq > lastAckSeq)
            changedObjs.push_back(pr.first);
    }
    writer << changedObjs.size();
    for (uint32_t id : changedObjs) {
        writer  << id;
        BaseStatus *cur = getStatus(id);
        BaseStatus *old = getStatus(id, lastAckSeq);

        // NOTE: we only worry about creature
        // changes at the moment
        if (old) {
            writeDelta(writer,
                *static_cast<CreatureStatus*>(cur),
                *static_cast<CreatureStatus*>(old));
        } else {
            CreatureStatus dummy;
            writeDelta(writer,
                *static_cast<CreatureStatus*>(cur),
                dummy);
        }
    }
    changedObjs.clear();

}

/*
* Only serialize the delta strings for all existing objects
* against dummy.
* All other temporary information such as ballistics and
* commands are ignored.
* 
* TODO: solve multithreading problems:
* multi-read-lock( serialize -> freeze ) -> release
* use threadpool to serialize concurrently for different
* clients
* 
* TODO: use TCP for this initial transmission, due to
* sheer size??
*/
void History::serialize(StreamWriter& writer) {
    // header
    writer << _seq << uint32_t(0) /*dummy*/ << getCurrent().tick;

    // list 1: object addition -> all objects currently in module
    writer << static_cast<uint8_t>(_area->objects().size());
    for (auto &obj : _area->objects()) {
        writer << static_cast<uint32_t>(obj->id()) 
               << obj->blueprintResRef();
    }
    // list 2: object deletion ... skip
    writer << uint8_t(0);

    // list 3: object delta string
    static CreatureStatus dummy = {};
    for (auto& obj : _area->objects()) {
        writer << obj->id();
        if (obj->type() == ObjectType::Creature) {
            writeDelta(writer,
                *static_cast<CreatureStatus*>(obj->captureStatus().get()),
                dummy);
        }
    }

    // list 4: inventory addition
    writer << uint8_t(0);
    //TODO: all of party inventory

    // list 5: inventory deletion ... skip
    writer << uint8_t(0);

    // list 6: ballistics ... skip
    writer << uint8_t(0);

    // list 7: animation commands ... skip
    writer << uint8_t(0);

    // list 8: additional commands ... skip
    writer << uint8_t(0);
}

void History::deserialize(StreamReader &reader, uint32_t lastAckSeq) {
    if (lastAckSeq + _maxLen <= _seq) return;

    uint32_t newSeq = 0, tick = 0;
    reader >> newSeq >> tick; // read header

    if (newSeq < _seq) return; // useless

    // fill in the gaps
    while (_seq < newSeq) {
        freeze(getMaster().tick + 1); //for potential binary search
    }
    getMaster().tick = tick;

    uint32_t id = 0;
    while (!reader.eof()) {
        reader >> id;

        // TODO: getSnapshot from creature itself
        auto stat = make_unique<CreatureStatus>();
        readDelta(reader, *stat);
        getMaster().objectStat[id] = move(stat);

        if (_activeObjects.count(id) != 0)
            _activeObjects[id] = {};

        _activeObjects[id].push_front(_seq); // newer to the front
    }
}

Snapshot &History::getCurrent() {
    return *_snapshots[_top];
}

Snapshot &History::getMaster() {
    return getSnapshot(_seq - 1);
}

Snapshot &History::getSnapshot(uint32_t seq) {
    if (seq + _maxLen <= _seq) throw out_of_range("Sequence out of bound");
    
    return *_snapshots[(_maxLen+_top-_seq+seq)%_maxLen];
}

BaseStatus *History::getStatus(uint32_t id) {
    if (_activeObjects.count(id) > 0) {
        return _snapshots[_activeObjects[id].front()]->objectStat[id].get();
    }

    if (_staleObjects.count(id) > 0) {
        return _staleObjects[id].get();
    }

    return nullptr;
}

BaseStatus *History::getStatus(uint32_t id, uint32_t seq) {
    if (_activeObjects.count(id) > 0) {
        // can replace with binary search
        for (uint32_t sq : _activeObjects[id]) {
            if (sq <= seq) {
                return getSnapshot(sq).objectStat[id].get();
            }
        }
    }

    if (_staleObjects.count(id) > 0) {
        return _staleObjects[id].get();
    }

    return nullptr;
}

} // namespace net

} // namespace reone
