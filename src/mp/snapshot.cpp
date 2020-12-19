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

using namespace std;

namespace reone {

namespace mp {

void History::insert(uint32_t id) {
    auto cptr = make_unique<CreatureStatus>();
    auto &master = *_snapshots.back();
    master.id_table[id] = make_unique<CreatureStatus>();
    // TODO: read status by type

    if (_activeObjects.count(id) != 0)
        _activeObjects[id] = {};

    _activeObjects[id].push_front(_seq); // newer to the front
}

void History::serialize(StreamWriter &writer, uint32_t lastAckSeq) {
    // write packet header
    writer << _seq << static_cast<uint32_t>(0); //TODO: global tick

    // disconnect/lost-packet, resend state of all objects
    if (lastAckSeq + _maxLen <= _seq) {
        for (const auto &pr : _activeObjects) {
            // TODO: read from id & compare against dummy
        }

        for (const auto &pr : _staleObjects) {
            // TODO: read from id & compare against dummy
        }
        return;
    }

    // generates diff
    for (const auto &pr : _activeObjects) { // pr.second always nonempty
        uint32_t latestSeq = pr.second.front();
        if (latestSeq <= lastAckSeq) continue;

        BaseStatus *cur = getStatus(pr.first);
        BaseStatus *old = getStatus(pr.first, lastAckSeq);

        writer << pr.first; // delta string header

        // TODO: different static_cast by obj id type
        if (old) writeDelta(writer,
                *static_cast<CreatureStatus*>(cur),
                *static_cast<CreatureStatus*>(old));
        else {
            CreatureStatus dummy; // TODO: need beter optimization
            writeDelta(writer,
                *static_cast<CreatureStatus*>(cur),
                dummy);
        }
    }
}

void History::deserialize(StreamReader &reader, uint32_t lastAckSeq) {
    if (lastAckSeq + _maxLen <= _seq) return;

    uint32_t newSeq = 0, tick = 0;
    reader >> newSeq >> tick; // read header

    if (newSeq < _seq) return; // useless

    // fill in the gaps
    while (_seq < newSeq) {
        addSnapshot(getMaster().tick + 1); //for potential binary search
    }
    getMaster().tick = tick;

    uint32_t id = 0;
    while (!reader.eof()) {
        reader >> id;

        // TODO: getSnapshot from creature itself
        auto stat = make_unique<CreatureStatus>();
        readDelta(reader, *stat);
        getMaster().id_table[id] = move(stat);

        if (_activeObjects.count(id) != 0)
            _activeObjects[id] = {};

        _activeObjects[id].push_front(_seq); // newer to the front
    }
}

Snapshot &History::getMaster() {
    return *_snapshots[_top];
}

Snapshot &History::getSnapshot(uint32_t seq) {
    if (seq + _maxLen <= _seq) throw out_of_range("Sequence out of bound");
    
    return *_snapshots[(_maxLen+_top-_seq+seq)%_maxLen];
}

BaseStatus *History::getStatus(uint32_t id) {
    if (_activeObjects.count(id) > 0) {
        return _snapshots[_activeObjects[id].front()]->id_table[id].get();
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
                return getSnapshot(sq).id_table[id].get();
            }
        }
    }

    if (_staleObjects.count(id) > 0) {
        return _staleObjects[id].get();
    }

    return nullptr;
}

void History::addSnapshot(uint32_t newTick) {
    ++_seq;

    if (_snapshots.size() < _maxLen) {
        _snapshots.push_back(make_unique<Snapshot>(_seq, newTick)); // TODO: add global tick!
        ++_top;
        return;
    }

    _top = (_top + 1) % _maxLen;

    // update activeObjects
    for (auto &pr : _snapshots[_top]->id_table) {
        if (_activeObjects[pr.first].back() <= _snapshots[_top]->seq) {
            _activeObjects[pr.first].pop_back();
        }

        if (_activeObjects[pr.first].empty()) {
            if (_staleObjects.count(pr.first) == 0) {
                _staleObjects[pr.first] = nullptr;
            }
            _staleObjects[pr.first].swap(pr.second);
            _activeObjects.erase(pr.first);
        }
    }

    _snapshots[_top]->reset(_seq, 0); // TODO: add global tick!
}

void Snapshot::reset(uint32_t newSeq, uint32_t newTick) {
    id_table.clear();
    
    seq = newSeq;
    tick = newTick;
}

} // namespace net

} // namespace reone
