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

#include "../node.h"

namespace reone {

namespace scene {

class EmitterSceneNode;

class ParticleSceneNode : public SceneNode {
public:
    ParticleSceneNode(
        const EmitterSceneNode &emitter,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        SceneNode(
            SceneNodeType::Particle,
            sceneGraph,
            graphicsSvc,
            audioSvc,
            resourceSvc),
        _emitter(emitter) {

        _point = false;
    }

    void update(float dt);

    bool isExpired() const;

    const glm::vec3 &dir() const { return _dir; }
    const glm::vec3 &color() const { return _color; }
    const glm::vec3 &velocity() const { return _velocity; }
    const glm::vec2 &size() const { return _size; }
    float animLength() const { return _animLength; }
    float lifetime() const { return _lifetime; }
    float alpha() const { return _alpha; }
    int frame() const { return _frame; }

    void setDir(glm::vec3 dir) { _dir = std::move(dir); }
    void setVelocity(glm::vec3 velocity) { _velocity = std::move(velocity); }
    void setSize(glm::vec2 size) { _size = std::move(size); }
    void setFrame(int frame) { _frame = frame; }
    void setAnimLength(float length) { _animLength = length; }
    void setLifetime(float lifetime) { _lifetime = lifetime; }

private:
    const EmitterSceneNode &_emitter;

    glm::vec3 _dir {0.0f}; // used in Linked render mode
    glm::vec3 _color {1.0f};
    glm::vec3 _velocity {0.0f};
    glm::vec2 _size {1.0f};
    float _animLength {0.0f};
    float _lifetime {0.0f};
    float _alpha {1.0f};
    int _frame {0};

    void updateAnimation(float dt);
};

} // namespace scene

} // namespace reone
