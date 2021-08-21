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

#include "../../graphics/texture/textures.h"
#include "../../resource/2da.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

#include "../game.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Area::loadARE(const GffStruct &are) {
    _localizedName = _game->services().resource().strings().get(are.getInt("Name"));

    loadCameraStyle(are);
    loadAmbientColor(are);
    loadScripts(are);
    loadMap(are);
    loadStealthXP(are);
    loadGrass(are);
    loadFog(are);
}

void Area::loadCameraStyle(const GffStruct &are) {
    shared_ptr<TwoDA> cameraStyles(_game->services().resource().resources().get2DA("camerastyle"));

    int areaStyleIdx = are.getInt("CameraStyle");
    _camStyleDefault.load(*cameraStyles, areaStyleIdx);

    int combatStyleIdx = cameraStyles->indexByCellValue("name", "Combat");
    if (combatStyleIdx != -1) {
        _camStyleCombat.load(*cameraStyles, combatStyleIdx);
    }
}

void Area::loadAmbientColor(const GffStruct &are) {
    _ambientColor = are.getColor("DynAmbientColor");
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
        _grass.texture = _game->services().graphics().textures().get(texName, TextureUsage::Diffuse);
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

void Area::loadFog(const GffStruct &are) {
    _fogEnabled = are.getBool("SunFogOn");
    _fogNear = are.getFloat("SunFogNear");
    _fogFar = are.getFloat("SunFogFar");
    _fogColor = are.getColor("SunFogColor");
}

} // namespace game

} // namespace reone
