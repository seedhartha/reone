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

#include "map.h"

#include "../../../common/log.h"
#include "../../../render/mesh/quad.h"
#include "../../../render/textures.h"

#include "../../game.h"

#include "../colors.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

MapMenu::MapMenu(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("map");
    _backgroundType = BackgroundType::Menu;

    if (game->version() == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
}

void MapMenu::load() {
    GUI::load();

    disableControl("BTN_PRTYSLCT");
    disableControl("BTN_RETURN");

    string arrowTex("mm_barrow");
    if (_version == GameVersion::TheSithLords) {
        arrowTex += "_p";
    }
    _arrow = Textures::instance().get(arrowTex, TextureType::GUI);
}

void MapMenu::update(float dt) {
    const ModuleInfo &info = _game->module()->info();
    string mapResRef("lbl_map" + info.entryArea);
    _map = Textures::instance().get(mapResRef, TextureType::GUI);
}

void MapMenu::render() const {
    GUI::render();

    if (!_map) return;

    drawMap();
    drawPartyLeader();
}

void MapMenu::drawMap() const {
    Control &label = getControl("LBL_Map");
    const Control::Extent &extent = label.extent();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_controlOffset.x + extent.left, _controlOffset.y + extent.top, 0.0f));
    transform = glm::scale(transform, glm::vec3(extent.width, extent.height, 1.0f));

    LocalUniforms locals;
    locals.general.model = transform;

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    _map->bind(0);

    Quad::getDefault().renderTriangles();
}

void MapMenu::drawPartyLeader() const {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    if (!partyLeader) return;

    const ModuleInfo &info = _game->module()->info();
    glm::vec3 leaderPos(partyLeader->position());
    float scaleX, scaleY, relX, relY;

    switch (info.northAxis) {
        case 0:
        case 1:
            scaleX = (info.mapPoint1.x - info.mapPoint2.x) / (info.worldPoint1.x - info.worldPoint2.x);
            scaleY = (info.mapPoint1.y - info.mapPoint2.y) / (info.worldPoint1.y - info.worldPoint2.y);
            relX = (leaderPos.x - info.worldPoint1.x) * scaleX + info.mapPoint1.x;
            relY = (leaderPos.y - info.worldPoint1.y) * scaleY + info.mapPoint1.y;
            break;
        case 2:
        case 3:
            scaleX = (info.mapPoint1.y - info.mapPoint2.y) / (info.worldPoint1.x - info.worldPoint2.x);
            scaleY = (info.mapPoint1.x - info.mapPoint2.x) / (info.worldPoint1.y - info.worldPoint2.y);
            relX = (leaderPos.y - info.worldPoint1.y) * scaleY + info.mapPoint1.x;
            relY = (leaderPos.x - info.worldPoint1.x) * scaleX + info.mapPoint1.y;
            break;
        default:
            warn("Map: invalid north axis: " + to_string(info.northAxis));
            return;
    }

    Control &label = getControl("LBL_Map");
    const Control::Extent &extent = label.extent();

    relX *= extent.width / static_cast<float>(_map->width());
    relY *= extent.height / static_cast<float>(_map->height());

    glm::vec3 arrowPos(
        _controlOffset.x + extent.left + relX * extent.width - 0.5f * _arrow->width(),
        _controlOffset.y + extent.top + relY * extent.height - 0.5f * _arrow->height(),
        0.0f);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, arrowPos);
    transform = glm::scale(transform, glm::vec3(_arrow->width(), _arrow->height(), 1.0f));

    LocalUniforms locals;
    locals.general.model = transform;

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    _arrow->bind(0);

    Quad::getDefault().renderTriangles();
}

void MapMenu::onClick(const string &control) {
    if (control == "BTN_EXIT") {
        _game->openInGame();
    }
}

} // namespace game

} // namespace reone
