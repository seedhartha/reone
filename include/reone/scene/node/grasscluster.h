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

class GrassClusterSceneNode : public SceneNode {
public:
    GrassClusterSceneNode(
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        SceneNode(
            SceneNodeType::GrassCluster,
            sceneGraph,
            graphicsSvc,
            audioSvc,
            resourceSvc) {
        _point = false;
    }

    const glm::vec2 &lightmapUV() const { return _lightmapUV; }
    int variant() const { return _variant; }

    void setLightmapUV(glm::vec2 uv) { _lightmapUV = std::move(uv); }
    void setVariant(int variant) { _variant = variant; }

private:
    glm::vec2 _lightmapUV {0.0f};
    int _variant {0};
};

} // namespace scene

} // namespace reone
