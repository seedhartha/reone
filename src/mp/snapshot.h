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

namespace game {

class Area;

}

namespace mp {

constexpr uint32_t kServerMaxLen = 64;
constexpr uint32_t kClientMaxLen = 3;  // may increase if too tight

struct Snapshot {
    Snapshot(uint32_t seq, uint32_t tick) : seq(seq), tick(tick) {};

    void reset(uint32_t newSeq, uint32_t newTick);

    Snapshot &operator=(const Snapshot &) = delete;
    //Snapshot &operator=(Snapshot&&) = delete;

    /* { id: resource_ref } */
    std::map<uint32_t, std::string> objectAddition;
    std::list<uint32_t> objectDeletion;

    /* { id: status } */
    std::map<uint32_t, std::unique_ptr<BaseStatus>> objectStat;

    /* { resource_ref: quantity }*/
    std::map<std::string, uint32_t> inventoryAddition;
    std::map<std::string, uint32_t> inventoryDeletion;

    /* id, animinfo */
    std::map<uint32_t, std::unique_ptr<AnimInfo>> animCommands;

    std::list<std::unique_ptr<Ballistic>> bullets;

    uint32_t seq;
    uint32_t tick;
};

/* 
* memory-efficient (cough cough) snapshot system
* sort of like a skip list
* 
* Server routine:
* 1. freeze(newTick)
* 2. serialize()
* 3. broadcast()
* 
* Client routine:
* 1. freeze()
* 2. deserialize()
*/
class History {
public:
    History(uint32_t maxLen, game::Area *area) : _maxLen(maxLen),
        _area(area) {
        _snapshots.reserve(_maxLen);
        _snapshots.push_back(std::make_unique<Snapshot>(++_seq, 0)); 
        // TODO: dummy tick
    }

    // creates a new snapshot
    void freeze(uint32_t newTick);

    // Server functions
    
    void addObject(uint32_t id, std::string resRef);
    void delObject(uint32_t id);

    void updateObject(uint32_t id);

    void addToInventory(std::string resRef, uint16_t quantity);
    void remFrInventory(std::string resRef, uint16_t quantity);
    
    void collectAnimCmds(); //server function

    void addBallistic(std::unique_ptr<Ballistic> &&bullet);

    /* 
    * Header:
    * uint32_t seq
    * uint32_t last ACK
    * uint32_t tick
    * 
    * Body serialize lists:
    * 1. objectAddition         x
    * 2. objectDeletion         x
    * 3. deltaStrings
    * 4. inventoryAddition      x
    * 5. inventoryDeletion      x
    * 6. bullets                x
    * 7. animation commands     x
    * 8. other commands         x
    * 
    * TODO: replace strings with int (via dynamic programming) to
    *   minimize transmission size, wherever possible.
    */
    void serialize(StreamWriter &writer, uint32_t lastAckSeq);
    void serialize(StreamWriter &writer); // from scratch

    // END Server functions

    // Client functions

    /* multiple StreamReader may be requried 
    * If newer seq, add new snapshot 
    * If older than _seq, discard
    */
    void deserialize(StreamReader &reader, uint32_t lastAckSeq);

    // END client functions

private:
    game::Area *_area;

    std::vector<std::unique_ptr<Snapshot>> _snapshots;
    uint32_t _top { 0 }; // rotated by _maxLen
    uint32_t _seq { 0 };
    uint32_t _maxLen;

    Snapshot &getCurrent(); // the one being built
    Snapshot &getMaster();  // last frozen snapshot
    Snapshot &getSnapshot(uint32_t seq);

    BaseStatus *getStatus(uint32_t id); //only latest
    BaseStatus *getStatus(uint32_t id, uint32_t seq);

    /* { id : [ lastSeq1, lastSeq2, ... (from new to old) ] }*/
    std::map<uint32_t, std::list<uint32_t>>         _activeObjects; // life cycle of obj up to getCurrent()
    std::map<uint32_t, std::unique_ptr<BaseStatus>> _staleObjects; /* last known state */
};

} // namespace net

} // namespace reone
