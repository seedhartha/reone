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
#include "../../render/model/modelnode.h"

namespace reone {

namespace scene {

class EmitterSceneNode : public SceneNode {
public:
    EmitterSceneNode(const std::shared_ptr<render::ModelNode> &modelNode, const std::shared_ptr<render::Emitter> &emitter, SceneGraph *sceneGraph);

    void update(float dt);

    void renderSingle(bool shadowPass) const override;

    std::shared_ptr<render::Emitter> emitter() const { return _emitter; }

private:
    struct Particle {
        glm::vec3 position { 0.0f };
        float cameraDistance { 0.0f };
        int frame { 0 };
        Timer frameTimer;
        glm::vec3 normal { 0.0f };
        float lifetime { 0.0f };
        float velocity { 0.0f };
    };

    std::shared_ptr<render::ModelNode> _modelNode;
    std::shared_ptr<render::Emitter> _emitter;

    float _birthInterval { 0.0f };
    Timer _birthTimer;
    std::vector<std::shared_ptr<Particle>> _particles;

    void init();

    void spawnParticles(float dt);
    void updateParticles(float dt);

    glm::vec3 getPlaneNormal() const;
};

} // namespace scene

} // namespace reone
