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

#include "modelnode.h"

namespace reone {

namespace scene {

class ModelSceneNode;
class ParticleSceneNode;

class EmitterSceneNode : public ModelNodeSceneNode {
public:
    EmitterSceneNode(
        const ModelSceneNode *model,
        std::shared_ptr<graphics::ModelNode> modelNode,
        SceneGraph &sceneGraph,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders);

    void update(float dt) override;
    void drawElements(const std::vector<SceneNode *> &elements, int count) override;

    void detonate();

    const graphics::BezierCurve<float> &particleSize() const { return _particleSize; }
    const graphics::BezierCurve<glm::vec3> &color() const { return _color; }
    const graphics::BezierCurve<float> &alpha() const { return _alpha; }

    float lifeExpectancy() const { return _lifeExpectancy; }
    int frameStart() const { return _frameStart; }
    int frameEnd() const { return _frameEnd; }
    float grav() const { return _grav; }

private:
    const ModelSceneNode *_model;

    graphics::BezierCurve<float> _particleSize;
    graphics::BezierCurve<glm::vec3> _color;
    graphics::BezierCurve<float> _alpha;

    float _birthrate {0.0f};      /**< rate of particle birth per second */
    float _lifeExpectancy {0.0f}; /**< life of each particle in seconds */
    glm::vec2 _size {0.0f};
    int _frameStart {0};
    int _frameEnd {0};
    float _fps {0.0f};
    float _spread {0.0f};
    float _velocity {0.0f};
    float _randomVelocity {0.0f};
    float _mass {0.0f};
    float _grav {0.0f};
    float _lightningDelay {0.0f};
    float _lightningRadius {0.0f};
    float _lightningScale {0.0f};
    int _lightningSubDiv {0};

    float _birthInterval {0.0f};
    Timer _birthTimer;
    bool _spawned {false};

    std::deque<std::shared_ptr<SceneNode>> _particlePool; /**< pre-allocated pool of particles */

    void spawnParticles(float dt);
    void removeExpiredParticles(float dt);
    void doSpawnParticle();
    void spawnLightningParticles();

    std::unique_ptr<ParticleSceneNode> newParticle();
};

} // namespace scene

} // namespace reone
