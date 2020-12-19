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
#pragma once

#include <cstdint>
#include <set>
#include <map>
#include <list>
#include <memory>
#include <vector>
#include <deque>

#include "objects.h"
#include "../common/streamreader.h"
#include "../common/streamwriter.h"

namespace reone {

namespace mp {

constexpr uint32_t kServerMaxLen = 64;
constexpr uint32_t kClientMaxLen = 3;  // may increase if too tight

struct Snapshot {
    Snapshot(uint32_t seq, uint32_t tick) : seq(seq), tick(tick) {};

    void reset(uint32_t newSeq, uint32_t newTick);

    Snapshot &operator=(const Snapshot &) = delete;
    //Snapshot &operator=(Snapshot&&) = delete;


    std::map<uint32_t, std::unique_ptr<BaseStatus>> id_table;
    uint32_t seq;
    uint32_t tick;
};

/* 
* memory-efficient (cough cough) snapshot system
* sort of like a skip list */
class History {
public:
    History(uint32_t maxLen) : _maxLen(maxLen) {
        _snapshots.reserve(_maxLen);
        _snapshots.push_back(std::make_unique<Snapshot>(_seq, 0)); // TODO: dummy tick
    }

    void addSnapshot(uint32_t newTick);

    // Server functions

    /* read status by type + insert to current snapshot */
    void insert(uint32_t id);

    /* serialize */
    void serialize(StreamWriter &writer, uint32_t lastAckSeq);

    // END Server functions

    // Client functions

    /* multiple StreamReader may be requried 
    * If newer seq, add new snapshot 
    * If older than _seq, discard */
    void deserialize(StreamReader &reader, uint32_t lastAckSeq);

    // END client functions

private:
    std::vector<std::unique_ptr<Snapshot>> _snapshots;
    uint32_t _top { 0 }; // rotated by _maxLen
    uint32_t _seq { 0 };
    uint32_t _maxLen;

    Snapshot &getMaster();
    Snapshot &getSnapshot(uint32_t seq);

    BaseStatus *getStatus(uint32_t id); //only latest
    BaseStatus *getStatus(uint32_t id, uint32_t seq);

    /* { id : [ lastSeq1, lastSeq2, ... (from new to old) ] }*/
    std::map<uint32_t, std::list<uint32_t>> _activeObjects;
    std::map<uint32_t, std::unique_ptr<BaseStatus>> _staleObjects;
};

} // namespace net

} // namespace reone
