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

#include "../mesh.h"
#include "../texture.h"

namespace reone {

namespace render {

class MdlFile;

/**
 * Corresponds to a Trimesh model node from an MDL model.
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

    void render();

    bool shouldRender() const { return _render; }
    bool shouldCastShadows() const { return _shadow; }

    bool isBackgroundGeometry() const { return _backgroundGeometry; }
    bool isBumpmapSwizzled() const { return _bumpmapSwizzled; }

    std::shared_ptr<Mesh> mesh() const { return _mesh; }
    int transparency() const { return _transparency; }
    const glm::vec3 &ambientColor() const { return _ambientColor; }
    const glm::vec3 &diffuseColor() const { return _diffuseColor; }
    const UVAnimation &uvAnimation() const { return _uvAnimation; }

    const std::shared_ptr<Texture> &diffuseTexture() const { return _diffuse; }
    const std::shared_ptr<Texture> &lightmapTexture() const { return _lightmap; }
    const std::shared_ptr<Texture> &bumpmapTexture() const { return _bumpmap; }

    void setRender(bool render);
    void setTransparency(int transparency);
    void setShadow(bool shadow);
    void setBackgroundGeometry(bool background);
    void setDiffuseTexture(std::shared_ptr<Texture> texture);
    void setBumpmapTexture(std::shared_ptr<Texture> texture, bool swizzled);
    void setDiffuseColor(glm::vec3 color);
    void setAmbientColor(glm::vec3 color);

private:
    std::shared_ptr<Mesh> _mesh;
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
    std::shared_ptr<Texture> _lightmap;
    std::shared_ptr<Texture> _bumpmap;

    // END Textures

    friend class MdlFile;
};

} // namespace render

} // namespace reone
