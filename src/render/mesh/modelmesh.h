/*
 * Copyright (c) 2020 The reone project contributors
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
    ModelMesh(bool render, int transparency);

    void render(const std::shared_ptr<Texture> &diffuseOverride = nullptr) const;

    bool shouldRender() const;
    bool isTransparent() const;
    bool hasDiffuseTexture() const;
    bool hasEnvmapTexture() const;
    bool hasLightmapTexture() const;
    bool hasBumpyShinyTexture() const;
    bool hasBumpmapTexture() const;

    int transparency() const;
    const std::shared_ptr<Texture> &diffuseTexture() const;

private:
    bool _render { false };
    int _transparency { 0 };
    std::shared_ptr<Texture> _diffuse;
    std::shared_ptr<Texture> _envmap;
    std::shared_ptr<Texture> _lightmap;
    std::shared_ptr<Texture> _bumpyShiny;
    std::shared_ptr<Texture> _bumpmap;

    friend class MdlFile;
};

} // namespace render

} // namespace reone
