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

#include <memory>

#include "../texture.h"

#include "mesh.h"

namespace reone {

namespace render {

class MdlFile;

/**
 * Textured mesh, part of a 3D model.
 *
 * @see reone::render::ModelNode
 * @see reone::render::Texture
 */
class ModelMesh : public Mesh {
public:
    struct UVAnimation {
        bool animated { false };
        float directionX { 0.0f };
        float directionY { 0.0f };
        float jitter { 0.0f };
        float jitterSpeed { 0.0f };
    };

    ModelMesh(bool render, int transparency, bool shadow);

    void render(const std::shared_ptr<Texture> &diffuseOverride = nullptr) const;

    bool shouldRender() const;
    bool shouldCastShadows() const;

    bool isTransparent() const;
    bool isBackgroundGeometry() const { return _backgroundGeometry; }

    bool hasDiffuseTexture() const;
    bool hasEnvmapTexture() const;
    bool hasLightmapTexture() const;
    bool hasBumpyShinyTexture() const;
    bool hasBumpmapTexture() const;

    int transparency() const;
    const glm::vec3 &diffuseColor() const { return _diffuseColor; }
    const glm::vec3 &ambientColor() const { return _ambientColor; }
    const UVAnimation &uvAnimation() const;

    const std::shared_ptr<Texture> &diffuseTexture() const;
    const std::shared_ptr<Texture> &bumpmapTexture() const;

private:
    bool _render { false };
    int _transparency { 0 };
    bool _shadow { false };
    bool _backgroundGeometry { false };
    glm::vec3 _diffuseColor { 0.0f };
    glm::vec3 _ambientColor { 0.0f };
    UVAnimation _uvAnimation;

    // Textures

    std::shared_ptr<Texture> _diffuse;
    std::shared_ptr<Texture> _envmap;
    std::shared_ptr<Texture> _lightmap;
    std::shared_ptr<Texture> _bumpyShiny;
    std::shared_ptr<Texture> _bumpmap;

    // END Textures

    friend class MdlFile;
};

} // namespace render

} // namespace reone
