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

/** @file
 *  Area functions related to loading ARE files.
 */

#include "area.h"

#include <boost/algorithm/string.hpp>

#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../game.h"

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

void Area::loadARE(const GffStruct &are) {
    _localizedName = Strings::instance().get(are.getInt("Name", -1));

    loadCameraStyle(are);
    loadAmbientColor(are);
    loadScripts(are);
    loadMap(are);
    loadStealthXP(are);
    loadGrass(are);
}

void Area::loadCameraStyle(const GffStruct &are) {
    shared_ptr<TwoDA> cameraStyles(Resources::instance().get2DA("camerastyle"));

    int areaStyleIdx = are.getInt("CameraStyle");
    _camStyleDefault.load(*cameraStyles, areaStyleIdx);

    int combatStyleIdx = cameraStyles->indexByCellValue("name", "Combat");
    if (combatStyleIdx != -1) {
        _camStyleCombat.load(*cameraStyles, combatStyleIdx);
    }
}

void Area::loadAmbientColor(const GffStruct &are) {
    int ambientColorValue = are.getInt("DynAmbientColor");
    glm::vec3 ambientColor(
        ambientColorValue & 0xff,
        (ambientColorValue >> 8) & 0xff,
        (ambientColorValue >> 16) & 0xff);

    ambientColor /= 255.0f;

    _game->sceneGraph().setAmbientLightColor(ambientColor);
}

void Area::loadScripts(const GffStruct &are) {
    _onEnter = are.getString("OnEnter");
    _onExit = are.getString("OnExit");
    _onHeartbeat = are.getString("OnHeartbeat");
    _onUserDefined = are.getString("OnUserDefined");
}

void Area::loadMap(const GffStruct &are) {
    _map.load(_name, *are.getStruct("Map"));
}

void Area::loadStealthXP(const GffStruct &are) {
    _stealthXPEnabled = are.getBool("StealthXPEnabled");
    _stealthXPDecrement = are.getInt("StealthXPLoss"); // TODO: loss = decrement?
    _maxStealthXP = are.getInt("StealthXPMax");
}

void Area::loadGrass(const GffStruct &are) {
    string texName(boost::to_lower_copy(are.getString("Grass_TexName")));
    if (!texName.empty()) {
        _grass.texture = Textures::instance().get(texName, TextureUsage::Diffuse);
    }
    _grass.density = are.getFloat("Grass_Density");
    _grass.quadSize = are.getFloat("Grass_QuadSize");
    _grass.ambient = are.getInt("Grass_Ambient");
    _grass.diffuse = are.getInt("Grass_Diffuse");
    _grass.probabilities[0] = are.getFloat("Grass_Prob_UL");
    _grass.probabilities[1] = are.getFloat("Grass_Prob_UR");
    _grass.probabilities[2] = are.getFloat("Grass_Prob_LL");
    _grass.probabilities[3] = are.getFloat("Grass_Prob_LR");
}

} // namespace game

} // namespace reone
