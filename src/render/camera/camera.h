/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "SDL2/SDL_events.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace reone {

namespace render {

class Camera {
public:
    virtual bool handle(const SDL_Event &event) = 0;
    virtual void update(float dt) = 0;
    virtual void resetInput() = 0;

    const glm::mat4 &projection() const;
    const glm::mat4 &view() const;
    const glm::vec3 &position() const;
    float heading() const;

protected:
    glm::mat4 _projection { 1.0f };
    glm::mat4 _view { 1.0f };
    glm::vec3 _position { 0.0f };
    float _heading { 0.0f };
};

} // namespace render

} // namespace reone
