/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/resource/gff.h"

#include "../object.h"
#include "../schema/git.h"
#include "../schema/ute.h"

namespace reone {

namespace game {

class Encounter : public Object {
public:
    Encounter(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Encounter,
            std::move(sceneName),
            game,
            services) {
    }

    void loadFromGIT(const schema::GIT_Encounter_List &git);

private:
    struct SpawnPoint {
        glm::vec3 position {0.0f};
        glm::quat orientation {1.0f, 0.0f, 0.0f, 0.0f};
    };

    struct EncounterCreature {
        int _appearance {0};
        float _cr {0.0f};
        std::string _resRef;
        bool _singleSpawn {false};
    };

    bool _active {false};
    int _difficultyIndex {0};
    Faction _faction {Faction::Invalid};
    int _maxCreatures {0};
    bool _playerOnly {false};
    int _recCreatures {0};
    bool _reset {false};
    int _resetTime {0};
    int _respawns {0};
    std::vector<EncounterCreature> _creatures;
    std::vector<glm::vec3> _geometry;
    std::vector<SpawnPoint> _spawnPoints;

    // Scripts

    std::string _onEntered;
    std::string _onExit;
    std::string _onExhausted;

    // END Scripts

    void loadFromBlueprint(const std::string &blueprintResRef);
    void loadPositionFromGIT(const schema::GIT_Encounter_List &gffs);
    void loadGeometryFromGIT(const schema::GIT_Encounter_List &gffs);
    void loadSpawnPointsFromGIT(const schema::GIT_Encounter_List &gffs);

    void loadUTE(const schema::UTE &ute);
    void loadCreaturesFromUTE(const schema::UTE &ute);
};

} // namespace game

} // namespace reone
