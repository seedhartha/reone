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

namespace reone {

namespace resource {

class GffStruct;

}

namespace game {

class Waypoint;

class IMap : boost::noncopyable {
public:
    enum class Mode {
        Default,
        Minimap
    };

    virtual ~IMap() = default;

    virtual void load(const std::string &area, const resource::GffStruct &gffs) = 0;

    virtual void draw(Mode mode, const glm::vec4 &bounds) = 0;

    virtual bool isLoaded() const = 0;

    virtual void setSelectedNote(std::shared_ptr<Waypoint> waypoint) = 0;
};

} // namespace game

} // namespace reone
