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

#include "scenenode.h"

#include <vector>

#include "glm/vec3.hpp"

#include "../../common/timer.h"
#include "../../render/emitter.h"

namespace reone {

namespace scene {

class ModelSceneNode;
class ParticleSceneNode;

class EmitterSceneNode : public SceneNode {
public:
    EmitterSceneNode(const ModelSceneNode *modelSceneNode, const std::shared_ptr<render::Emitter> &emitter, SceneGraph *sceneGraph);

    void update(float dt) override;

    void detonate();

    std::shared_ptr<render::Emitter> emitter() const { return _emitter; }

private:
    const ModelSceneNode *_modelSceneNode;
    std::shared_ptr<render::Emitter> _emitter;

    float _birthInterval { 0.0f };
    Timer _birthTimer;
    std::vector<std::shared_ptr<ParticleSceneNode>> _particles;
    bool _spawned { false };

    void init();

    void spawnParticles(float dt);
    void removeExpiredParticles(float dt);

    void doSpawnParticle();
};

} // namespace scene

} // namespace reone
