/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "spatial.h"

#include "../../resource/gffstruct.h"

namespace reone {

namespace game {

class Encounter : public SpatialObject {
public:
    Encounter(
        uint32_t id,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        ScriptRunner *scriptRunner);

    void loadFromGIT(const resource::GffStruct &gffs);

private:
    struct SpawnPoint {
        glm::vec3 position { 0.0f };
        glm::quat orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    };

    struct EncounterCreature {
        int _appearance { 0 };
        float _cr { 0.0f };
        std::string _resRef;
        bool _singleSpawn { false };
    };

    bool _active { false };
    int _difficulty { 0 };
    int _difficultyIndex { 0 };
    Faction _faction { Faction::Invalid };
    int _maxCreatures { 0 };
    bool _playerOnly { false };
    int _recCreatures { 0 };
    bool _reset { false };
    int _resetTime { 0 };
    int _respawns { 0 };
    std::vector<EncounterCreature> _creatures;
    std::vector<glm::vec3> _geometry;
    std::vector<SpawnPoint> _spawnPoints;

    // Scripts

    std::string _onEntered;
    std::string _onExit;
    std::string _onExhausted;

    // END Scripts

    void loadFromBlueprint(const std::string &blueprintResRef);
    void loadPositionFromGIT(const resource::GffStruct &gffs);
    void loadGeometryFromGIT(const resource::GffStruct &gffs);
    void loadSpawnPointsFromGIT(const resource::GffStruct &gffs);

    void loadUTE(const resource::GffStruct &ute);
    void loadCreaturesFromUTE(const resource::GffStruct &ute);
};

} // namespace game

} // namespace reone
