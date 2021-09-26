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

#include "../../common/timer.h"
#include "../../graphics/beziercurve.h"

#include "../nodeelement.h"

#include "modelnode.h"

namespace reone {

namespace scene {

class ModelSceneNode;

class EmitterSceneNode : public ModelNodeSceneNode {
public:
    struct Particle : public SceneNodeElement {
        glm::vec3 position { 0.0f };
        glm::vec3 dir { 0.0f }; // used in Linked render mode
        glm::vec3 color { 1.0f };
        glm::vec3 velocity { 0.0f };
        glm::vec2 size { 1.0f };
        float animLength { 0.0f };
        float lifetime { 0.0f };
        float alpha { 1.0f };
        int frame { 0 };
    };

    EmitterSceneNode(
        const ModelSceneNode *model,
        std::shared_ptr<graphics::ModelNode> modelNode,
        SceneGraph &sceneGraph,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders);

    void update(float dt) override;
    void drawElements(const std::vector<std::shared_ptr<SceneNodeElement>> &elements, int count) override;

    void detonate();

    const std::deque<std::shared_ptr<Particle>> &particles() const { return _particles; }

private:
    const ModelSceneNode *_model;

    graphics::BezierCurve<float> _particleSize;
    graphics::BezierCurve<glm::vec3> _color;
    graphics::BezierCurve<float> _alpha;

    float _birthrate { 0.0f }; /**< rate of particle birth per second */
    float _lifeExpectancy { 0.0f }; /**< life of each particle in seconds */
    glm::vec2 _size { 0.0f };
    int _frameStart { 0 };
    int _frameEnd { 0 };
    float _fps { 0.0f };
    float _spread { 0.0f };
    float _velocity { 0.0f };
    float _randomVelocity { 0.0f };
    float _mass { 0.0f };
    float _grav { 0.0f };
    float _lightningDelay { 0.0f };
    float _lightningRadius { 0.0f };
    float _lightningScale { 0.0f };
    int _lightningSubDiv { 0 };

    float _birthInterval { 0.0f };
    Timer _birthTimer;
    std::deque<std::shared_ptr<Particle>> _particles;
    bool _spawned { false };

    void spawnParticles(float dt);
    void removeExpiredParticles(float dt);
    void doSpawnParticle();
    void spawnLightningParticles();

    void updateParticle(Particle &particle, float dt);
    void updateParticleAnimation(Particle &particle, float dt);

    bool isParticleExpired(Particle &particle) const;
};

} // namespace scene

} // namespace reone
