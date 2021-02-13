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

#include "../material.h"
#include "../texture.h"

#include "mesh.h"

namespace reone {

namespace render {

class MdlFile;

/**
 * Textured mesh, part of a 3D model node.
 *
 * @see reone::render::ModelNode
 */
class ModelMesh {
public:
    struct UVAnimation {
        bool animated { false };
        float directionX { 0.0f };
        float directionY { 0.0f };
        float jitter { 0.0f };
        float jitterSpeed { 0.0f };
    };

    ModelMesh(const std::shared_ptr<Mesh> &mesh);

    void initGL();
    void deinitGL();

    void render(std::shared_ptr<Texture> diffuse = nullptr) const;

    bool shouldRender() const { return _render; }
    bool shouldCastShadows() const { return _shadow; }

    bool isTransparent() const;
    bool isBackgroundGeometry() const { return _backgroundGeometry; }
    bool isBumpmapSwizzled() const { return _bumpmapSwizzled; }

    bool hasDiffuseTexture() const { return static_cast<bool>(_diffuse); }
    bool hasEnvmapTexture() const { return static_cast<bool>(_envmap); }
    bool hasLightmapTexture() const { return static_cast<bool>(_lightmap); }
    bool hasBumpmapTexture() const { return static_cast<bool>(_bumpmap); }

    std::shared_ptr<Mesh> mesh() const { return _mesh; }
    const Material &material() const { return _material; }
    int transparency() const { return _transparency; }
    const glm::vec3 &ambientColor() const { return _ambientColor; }
    const glm::vec3 &diffuseColor() const { return _diffuseColor; }
    const UVAnimation &uvAnimation() const { return _uvAnimation; }

    const std::shared_ptr<Texture> &diffuseTexture() const { return _diffuse; }
    const std::shared_ptr<Texture> &envmapTexture() const { return _envmap; }
    const std::shared_ptr<Texture> &bumpmapTexture() const { return _bumpmap; }

    void setRender(bool render);
    void setTransparency(int transparency);
    void setShadow(bool shadow);
    void setBackgroundGeometry(bool background);
    void setDiffuseTexture(const std::shared_ptr<Texture> &texture);
    void setBumpmapTexture(const std::shared_ptr<Texture> &texture, bool swizzled = false);
    void setDiffuseColor(glm::vec3 color);
    void setAmbientColor(glm::vec3 color);

private:
    std::shared_ptr<Mesh> _mesh;
    Material _material;
    UVAnimation _uvAnimation;

    int _transparency { 0 };
    glm::vec3 _ambientColor { 1.0f };
    glm::vec3 _diffuseColor { 0.0f };

    bool _render { false };
    bool _shadow { false };
    bool _backgroundGeometry { false };
    bool _bumpmapSwizzled { false };

    // Textures

    std::shared_ptr<Texture> _diffuse;
    std::shared_ptr<Texture> _envmap;
    std::shared_ptr<Texture> _lightmap;
    std::shared_ptr<Texture> _bumpmap;

    // END Textures

    friend class MdlFile;
};

} // namespace render

} // namespace reone
