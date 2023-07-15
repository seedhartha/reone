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

#include "reone/game/object/encounter.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/resource/di/services.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"

using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Encounter::loadFromGIT(const schema::GIT_Encounter_List &git) {
    std::string blueprintResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(blueprintResRef);

    loadPositionFromGIT(git);
    loadGeometryFromGIT(git);
}

void Encounter::loadFromBlueprint(const std::string &blueprintResRef) {
    std::shared_ptr<Gff> ute(_services.resource.gffs.get(blueprintResRef, ResourceType::Ute));
    if (ute) {
        auto uteParsed = schema::parseUTE(*ute);
        loadUTE(uteParsed);
    }
}

void Encounter::loadPositionFromGIT(const schema::GIT_Encounter_List &git) {
    float x = git.XPosition;
    float y = git.YPosition;
    float z = git.ZPosition;
    _position = glm::vec3(x, y, z);
    updateTransform();
}

void Encounter::loadGeometryFromGIT(const schema::GIT_Encounter_List &git) {
    for (auto &pointStruct : git.Geometry) {
        float x = pointStruct.X;
        float y = pointStruct.Y;
        float z = pointStruct.Z;
        _geometry.push_back(glm::vec3(x, y, z));
    }
}

void Encounter::loadSpawnPointsFromGIT(const schema::GIT_Encounter_List &git) {
    for (auto &pointStruct : git.SpawnPointList) {
        float x = pointStruct.X;
        float y = pointStruct.Y;
        float z = pointStruct.Z;
        float orientation = pointStruct.Orientation;

        SpawnPoint point;
        point.position = glm::vec3(x, y, z);
        point.orientation = glm::angleAxis(orientation, glm::vec3(0.0f, 0.0f, 1.0f)); // TODO: validate
        _spawnPoints.push_back(std::move(point));
    }
}

void Encounter::loadUTE(const schema::UTE &ute) {
    _tag = boost::to_lower_copy(ute.Tag);
    _name = _services.resource.strings.getText(ute.LocalizedName.first);
    _blueprintResRef = boost::to_lower_copy(ute.TemplateResRef);
    _active = ute.Active;
    _difficultyIndex = ute.DifficultyIndex; // index into encdifficulty.2da
    _faction = static_cast<Faction>(ute.Faction);
    _maxCreatures = ute.MaxCreatures;
    _playerOnly = ute.PlayerOnly;
    _reset = ute.Reset;
    _resetTime = ute.ResetTime;
    _respawns = ute.Respawns;

    _onEntered = ute.OnEntered;
    _onExit = ute.OnExit;               // always empty, but could be useful
    _onExhausted = ute.OnExhausted;     // always empty, but could be useful
    _onHeartbeat = ute.OnHeartbeat;     // always empty, but could be useful
    _onUserDefined = ute.OnUserDefined; // always empty, but could be useful

    loadCreaturesFromUTE(ute);

    // Unused fields:
    //
    // - Difficulty (obsolete)
    // - SpawnOption (always 1)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Encounter::loadCreaturesFromUTE(const schema::UTE &ute) {
    for (auto &creatureStruct : ute.CreatureList) {
        EncounterCreature creature;
        creature._appearance = creatureStruct.Appearance;
        creature._cr = creatureStruct.CR;
        creature._resRef = creatureStruct.ResRef;
        creature._singleSpawn = creatureStruct.SingleSpawn;
        _creatures.push_back(std::move(creature));
    }
}

} // namespace game

} // namespace reone
