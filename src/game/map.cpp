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

#include "map.h"

#include <stdexcept>

#include "glm/mat4x4.hpp"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../common/log.h"
#include "../render/mesh/quad.h"
#include "../render/stateutil.h"
#include "../render/textures.h"
#include "../resource/types.h"

#include "game.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kArrowSize = 32;

Map::Map(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("game must not be null");
    }
}

void Map::load(const string &area, const GffStruct &gffs) {
    loadProperties(gffs);
    loadTexture(area);

    if (!_arrow) {
        loadArrow();
    }
}

void Map::loadProperties(const GffStruct &gffs) {
    _northAxis = gffs.getInt("NorthAxis");
    _worldPoint1 = glm::vec2(gffs.getFloat("WorldPt1X"), gffs.getFloat("WorldPt1Y"));
    _worldPoint2 = glm::vec2(gffs.getFloat("WorldPt2X"), gffs.getFloat("WorldPt2Y"));
    _mapPoint1 = glm::vec2(gffs.getFloat("MapPt1X"), gffs.getFloat("MapPt1Y"));
    _mapPoint2 = glm::vec2(gffs.getFloat("MapPt2X"), gffs.getFloat("MapPt2Y"));
}

void Map::loadTexture(const string &area) {
    string resRef("lbl_map" + area);
    _texture = Textures::instance().get(resRef, TextureType::GUI);
}

void Map::loadArrow() {
    string resRef("mm_barrow");
    if (_game->gameId() == GameID::TSL) {
        resRef += "_p";
    }
    _arrow = Textures::instance().get(resRef, TextureType::GUI);
}

void Map::render(Mode mode, const glm::vec4 &bounds) const {
    if (!_texture) return;

    drawArea(mode, bounds);
    drawPartyLeader(mode, bounds);
}

void Map::drawArea(Mode mode, const glm::vec4 &bounds) const {
    if (mode == Mode::Minimap) {
        shared_ptr<Creature> partyLeader(_game->party().getLeader());
        if (!partyLeader) return;

        glm::vec2 worldPos(partyLeader->position());
        glm::vec2 mapPos(getMapPosition(worldPos));

        glm::vec3 topLeft(0.0f);
        topLeft.x = bounds[0] + 0.5f * bounds[2] - mapPos.x * 440.0f / static_cast<float>(_texture->width()) * _texture->width();
        topLeft.y = bounds[1] + 0.5f * bounds[3] - mapPos.y * _texture->height();

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, topLeft);
        transform = glm::scale(transform, glm::vec3(_texture->width(), _texture->height(), 1.0f));

        LocalUniforms locals;
        locals.general.model = transform;

        Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

        setActiveTextureUnit(0);
        _texture->bind();

        float height = _game->options().graphics.height;
        glm::ivec4 scissorBounds(bounds[0], height - (bounds[1] + bounds[3]), bounds[2], bounds[3]);
        withScissorTest(scissorBounds, []() { Quad::getDefault().renderTriangles(); });

    } else {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(bounds[0], bounds[1], 0.0f));
        transform = glm::scale(transform, glm::vec3(bounds[2], bounds[3], 1.0f));

        LocalUniforms locals;
        locals.general.model = transform;

        Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

        setActiveTextureUnit(0);
        _texture->bind();

        Quad::getDefault().renderTriangles();
    }
}

void Map::drawPartyLeader(Mode mode, const glm::vec4 &bounds) const {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    if (!partyLeader) return;

    glm::vec3 arrowPos(0.0f);

    if (mode == Mode::Default) {
        glm::vec2 worldPos(partyLeader->position());
        glm::vec2 mapPos(getMapPosition(worldPos));

        mapPos.x *= bounds[2] / static_cast<float>(_texture->width());
        mapPos.y *= bounds[3] / static_cast<float>(_texture->height());

        arrowPos.x = bounds[0] + mapPos.x * bounds[2];
        arrowPos.y = bounds[1] + mapPos.y * bounds[3];

    } else {
        arrowPos.x = bounds[0] + 0.5f * bounds[2];
        arrowPos.y = bounds[1] + 0.5f * bounds[3];
    }

    float facing;
    switch (_northAxis) {
        case 0:
            facing = -partyLeader->facing();
            break;
        case 1:
            facing = glm::pi<float>() - partyLeader->facing();
            break;
        case 2:
            facing = glm::three_over_two_pi<float>() - partyLeader->facing();
            break;
        default:
            facing = glm::half_pi<float>() - partyLeader->facing();
            break;
    }
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, arrowPos);
    transform = glm::rotate(transform, facing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(-0.5f * kArrowSize, -0.5f * kArrowSize, 0.0f));
    transform = glm::scale(transform, glm::vec3(kArrowSize, kArrowSize, 1.0f));

    LocalUniforms locals;
    locals.general.model = transform;

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    setActiveTextureUnit(0);
    _arrow->bind();

    Quad::getDefault().renderTriangles();
}

glm::vec2 Map::getMapPosition(const glm::vec2 &world) const {
    float scaleX, scaleY;
    glm::vec2 result(0.0f);

    switch (_northAxis) {
        case 0:
        case 1:
            scaleX = (_mapPoint1.x - _mapPoint2.x) / (_worldPoint1.x - _worldPoint2.x);
            scaleY = (_mapPoint1.y - _mapPoint2.y) / (_worldPoint1.y - _worldPoint2.y);
            result.x = (world.x - _worldPoint1.x) * scaleX + _mapPoint1.x;
            result.y = (world.y - _worldPoint1.y) * scaleY + _mapPoint1.y;
            break;
        case 2:
        case 3:
            scaleX = (_mapPoint1.y - _mapPoint2.y) / (_worldPoint1.x - _worldPoint2.x);
            scaleY = (_mapPoint1.x - _mapPoint2.x) / (_worldPoint1.y - _worldPoint2.y);
            result.x = (world.y - _worldPoint1.y) * scaleY + _mapPoint1.x;
            result.y = (world.x - _worldPoint1.x) * scaleX + _mapPoint1.y;
            break;
        default:
            warn("Map: invalid north axis: " + to_string(_northAxis));
            break;
    }

    return move(result);
}

} // namespace game

} // namespace reone
