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

#include "glm/vec3.hpp"

namespace reone {

namespace scene {

class EmitterSceneNode;

class Particle {
public:
    Particle(glm::vec3 position, float velocity, EmitterSceneNode *emitter);

    void update(float dt);

    bool isExpired() const;

    EmitterSceneNode *emitter() const { return _emitter; }
    const glm::vec3 &position() const { return _position; }
    float size() const { return _size; }
    int frame() const { return _frame; }
    const glm::vec3 &color() const { return _color; }
    float alpha() const { return _alpha; }

private:
    glm::vec3 _position;
    float _velocity;
    EmitterSceneNode *_emitter;

    float _animLength { 0.0f };
    int _renderOrder { 0 };
    float _lifetime { 0.0f };
    int _frame { 0 };
    float _size { 1.0f };
    glm::vec3 _color { 1.0f };
    float _alpha { 1.0f };

    void init();
    void updateAnimation(float dt);
};

} // namespace scene

} // namespace reone
