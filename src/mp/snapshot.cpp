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
    activeObjects.clear();

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

    // for (auto &pr : toDelete->objectStat) {
    //     auto &seqList = _activeObjects[pr.first];
    //     while (seqList.back() <= toDelete->seq) {
    //         seqList.pop_back();
    //     }
    //     if (seqList.empty()) {
    //         _activeObjects.erase(pr.first);
    //     }

    //     _staleObjects[pr.first] = std::move(pr.second);
    //     pr.second = nullptr;
    // }

    _snapshots[_top]->reset(_seq, 0); // TODO: add global tick!
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
}

void History::deserialize(StreamReader &reader, uint32_t lastAckSeq) {
    if (lastAckSeq + _maxLen <= _seq) return;

    uint32_t newSeq = 0, tick = 0;
    reader >> newSeq >> tick; // read header

    if (newSeq < _seq) return; // useless

    // fill in the gaps
    while (_seq < newSeq) {
        freeze(getStable().tick + 1); //for potential binary search
    }
    getStable().tick = tick;

    uint32_t id = 0;
    while (!reader.eof()) {
        reader >> id;

        // TODO: getSnapshot from creature itself
        auto stat = make_unique<CreatureStatus>();
        readDelta(reader, *stat);
    }
}

Snapshot &History::getCurrent() {
    return *_snapshots[_top];
}

Snapshot &History::getSnapshot(uint32_t seq) {
    if (seq + _maxLen <= _seq) throw out_of_range("Sequence out of bound");
    
    return *_snapshots[(_maxLen+_top-_seq+seq)%_maxLen];
}

} // namespace net

} // namespace reone
