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

#include "reone/graphics/texture.h"

#include "../schema/are.h"

namespace reone {

namespace resource {

class Gff;

}

namespace game {

struct ServicesView;

class Game;
class Waypoint;

class Map {
public:
    enum class Mode {
        Default,
        Minimap
    };

    Map(Game &game, ServicesView &services);

    void load(const std::string &area, const schema::ARE_Map &map);

    void draw(Mode mode, const glm::vec4 &bounds);

    bool isLoaded() const { return static_cast<bool>(_areaTexture); }

    void setSelectedNote(std::shared_ptr<Waypoint> waypoint) { _selectedNote = std::move(waypoint); }

private:
    Game &_game;
    ServicesView &_services;

    int _northAxis {0};
    glm::vec2 _worldPoint1 {0.0f};
    glm::vec2 _worldPoint2 {0.0f};
    glm::vec2 _mapPoint1 {0.0f};
    glm::vec2 _mapPoint2 {0.0f};

    std::shared_ptr<graphics::Texture> _areaTexture;
    std::shared_ptr<graphics::Texture> _arrowTexture;
    std::shared_ptr<graphics::Texture> _noteTexture;

    std::string _arrowResRef;
    std::shared_ptr<Waypoint> _selectedNote;

    void loadProperties(const schema::ARE_Map &map);
    void loadTextures(const std::string &area);

    void drawArea(Mode mode, const glm::vec4 &bounds);
    void drawPartyLeader(Mode mode, const glm::vec4 &bounds);
    void drawNotes(Mode mode, const glm::vec4 &bounds);

    glm::vec2 getMapPosition(const glm::vec2 &world) const;
};

} // namespace game

} // namespace reone
