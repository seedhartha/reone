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
#include "../../graphics/model/modelnode.h"

#include "modelnodescenenode.h"

namespace reone {

namespace scene {

class ModelSceneNode;

class EmitterSceneNode : public ModelNodeSceneNode {
public:
    struct Particle {
        EmitterSceneNode *emitter { nullptr };
        glm::vec3 position { 0.0f };
        glm::vec3 color { 1.0f };
        glm::vec3 velocity { 0.0f };
        float animLength { 0.0f };
        float lifetime { 0.0f };
        int frame { 0 };
        float size { 1.0f };
        float alpha { 1.0f };
    };

    template <class T>
    struct Constraints {
        T start;
        T mid;
        T end;
    };

    EmitterSceneNode(const ModelSceneNode *model, std::shared_ptr<graphics::ModelNode> modelNode, SceneGraph *sceneGraph);

    void update(float dt) override;
    void drawParticles(const std::vector<Particle *> &particles);

    void detonate();

    const std::vector<std::shared_ptr<Particle>> &particles() const { return _particles; }

private:
    const ModelSceneNode *_model;

    Constraints<float> _particleSize;
    Constraints<glm::vec3> _color;
    Constraints<float> _alpha;

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

    float _birthInterval { 0.0f };
    Timer _birthTimer;
    std::vector<std::shared_ptr<Particle>> _particles;
    bool _spawned { false };

    void spawnParticles(float dt);
    void removeExpiredParticles(float dt);
    void doSpawnParticle();

    void initParticle(Particle &particle);
    void updateParticle(Particle &particle, float dt);
    void updateParticleAnimation(Particle &particle, float dt);

    bool isParticleExpired(Particle &particle) const;
};

} // namespace scene

} // namespace reone
