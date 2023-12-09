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

#include "reone/game/gui/map.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/resource/gff.h"
#include "reone/resource/provider/textures.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kArrowSize = 32;
static constexpr int kMapNoteSize = 16;
static constexpr float kSelectedMapNoteScale = 1.5f;

Map::Map(Game &game, ServicesView &services) :
    _game(game),
    _services(services) {
    if (game.isTSL()) {
        _arrowResRef = "mm_barrow_p";
    } else {
        _arrowResRef = "mm_barrow";
    }
}

void Map::load(const std::string &area, const resource::generated::ARE_Map &map) {
    loadProperties(map);
    loadTextures(area);
}

void Map::loadProperties(const resource::generated::ARE_Map &map) {
    _northAxis = map.NorthAxis;
    _worldPoint1 = glm::vec2(map.WorldPt1X, map.WorldPt1Y);
    _worldPoint2 = glm::vec2(map.WorldPt2X, map.WorldPt2Y);
    _mapPoint1 = glm::vec2(map.MapPt1X, map.MapPt1Y);
    _mapPoint2 = glm::vec2(map.MapPt2X, map.MapPt2Y);
}

void Map::loadTextures(const std::string &area) {
    std::string resRef("lbl_map" + area);
    _areaTexture = _services.resource.textures.get(resRef, TextureUsage::GUI);

    if (!_arrowTexture) {
        _arrowTexture = _services.resource.textures.get(_arrowResRef, TextureUsage::GUI);
    }
    if (!_noteTexture) {
        _noteTexture = _services.resource.textures.get("whitetarget", TextureUsage::GUI);
    }
}

void Map::render(Mode mode, const glm::vec4 &bounds) {
    if (!_areaTexture) {
        return;
    }
    _services.graphics.context.withBlending(BlendMode::Normal, [this, &mode, &bounds]() {
        renderArea(mode, bounds);
        renderNotes(mode, bounds);
        renderPartyLeader(mode, bounds);
    });
}

void Map::renderArea(Mode mode, const glm::vec4 &bounds) {
    if (mode == Mode::Minimap) {
        std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
        if (!partyLeader) {
            return;
        }
        _services.graphics.context.bindTexture(*_areaTexture);

        glm::vec2 worldPos(partyLeader->position());
        glm::vec2 mapPos(getMapPosition(worldPos));

        glm::vec3 topLeft(0.0f);
        topLeft.x = bounds[0] + 0.5f * bounds[2] - mapPos.x * 440.0f / static_cast<float>(_areaTexture->width()) * _areaTexture->width();
        topLeft.y = bounds[1] + 0.5f * bounds[3] - mapPos.y * _areaTexture->height();

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, topLeft);
        transform = glm::scale(transform, glm::vec3(_areaTexture->width(), _areaTexture->height(), 1.0f));

        _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
            globals.projection = _services.graphics.window.getOrthoProjection();
        });
        _services.graphics.uniforms.setLocals([transform](auto &locals) {
            locals.reset();
            locals.model = std::move(transform);
        });
        _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture));

        int height = _game.options().graphics.height;
        glm::ivec4 scissorBounds(bounds[0], height - (bounds[1] + bounds[3]), bounds[2], bounds[3]);
        _services.graphics.context.withScissorTest(scissorBounds, [&]() {
            _services.graphics.meshRegistry.get(MeshName::quad).draw();
        });

    } else {
        _services.graphics.context.bindTexture(*_areaTexture);

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(bounds[0], bounds[1], 0.0f));
        transform = glm::scale(transform, glm::vec3(bounds[2], bounds[3], 1.0f));

        _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
            globals.projection = _services.graphics.window.getOrthoProjection();
        });
        _services.graphics.uniforms.setLocals([transform](auto &locals) {
            locals.reset();
            locals.model = std::move(transform);
        });
        _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture));
        _services.graphics.meshRegistry.get(MeshName::quad).draw();
    }
}

void Map::renderNotes(Mode mode, const glm::vec4 &bounds) {
    if (mode != Mode::Default) {
        return;
    }
    _services.graphics.context.bindTexture(*_noteTexture);

    for (auto &object : _game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        auto waypoint = std::static_pointer_cast<Waypoint>(object);
        if (!waypoint->isMapNoteEnabled() || waypoint->mapNote().empty())
            continue;

        glm::vec2 mapPos(getMapPosition(waypoint->position()));
        mapPos.x *= bounds[2] / static_cast<float>(_areaTexture->width());
        mapPos.y *= bounds[3] / static_cast<float>(_areaTexture->height());

        glm::vec2 notePos;
        notePos.x = bounds[0] + mapPos.x * bounds[2];
        notePos.y = bounds[1] + mapPos.y * bounds[3];

        bool selected = waypoint == _selectedNote;
        float noteSize = (selected ? kSelectedMapNoteScale : 1.0f) * kMapNoteSize;

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(notePos.x - 0.5f * noteSize, notePos.y - 0.5f * noteSize, 0.0f));
        transform = glm::scale(transform, glm::vec3(noteSize, noteSize, 1.0f));

        auto guiColorHilight = _game.isTSL() ? kTSLGUIColorHilight : kGeometryUIColorHilight;
        auto guiColorBase = _game.isTSL() ? kTSLGUIColorBase : kGeometryUIColorBase;

        _services.graphics.uniforms.setGlobals([&](auto &globals) {
            globals.projection = _services.graphics.window.getOrthoProjection();
        });
        _services.graphics.uniforms.setLocals([&](auto &locals) {
            locals.reset();
            locals.model = std::move(transform);
            locals.color = glm::vec4(selected ? guiColorHilight : guiColorBase, 1.0f);
        });
        _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture));
        _services.graphics.meshRegistry.get(MeshName::quad).draw();
    }
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
        warn("Map: invalid north axis: " + std::to_string(_northAxis));
        break;
    }

    return result;
}

void Map::renderPartyLeader(Mode mode, const glm::vec4 &bounds) {
    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
    if (!partyLeader) {
        return;
    }
    _services.graphics.context.bindTexture(*_arrowTexture);

    glm::vec3 arrowPos(0.0f);

    if (mode == Mode::Default) {
        glm::vec2 worldPos(partyLeader->position());
        glm::vec2 mapPos(getMapPosition(worldPos));

        mapPos.x *= bounds[2] / static_cast<float>(_areaTexture->width());
        mapPos.y *= bounds[3] / static_cast<float>(_areaTexture->height());

        arrowPos.x = bounds[0] + mapPos.x * bounds[2];
        arrowPos.y = bounds[1] + mapPos.y * bounds[3];

    } else {
        arrowPos.x = bounds[0] + 0.5f * bounds[2];
        arrowPos.y = bounds[1] + 0.5f * bounds[3];
    }

    float facing;
    switch (_northAxis) {
    case 0:
        facing = -partyLeader->getFacing();
        break;
    case 1:
        facing = glm::pi<float>() - partyLeader->getFacing();
        break;
    case 2:
        facing = glm::three_over_two_pi<float>() - partyLeader->getFacing();
        break;
    default:
        facing = glm::half_pi<float>() - partyLeader->getFacing();
        break;
    }
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, arrowPos);
    transform = glm::rotate(transform, facing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(-0.5f * kArrowSize, -0.5f * kArrowSize, 0.0f));
    transform = glm::scale(transform, glm::vec3(kArrowSize, kArrowSize, 1.0f));

    _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _services.graphics.window.getOrthoProjection();
    });
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

} // namespace game

} // namespace reone
