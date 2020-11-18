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

#include "modelmesh.h"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

using namespace std;

namespace reone {

namespace render {

ModelMesh::ModelMesh(bool render, int transparency) : _render(render), _transparency(transparency) {
}

void ModelMesh::render(const shared_ptr<Texture> &diffuseOverride) const {
    const shared_ptr<Texture> &diffuse = diffuseOverride ? diffuseOverride : _diffuse;
    bool additive = false;

    if (diffuse) {
        diffuse->bind(0);
        additive = diffuse->isAdditive();
    }
    if (_envmap) {
        _envmap->bind(1);
    }
    if (_lightmap) {
        _lightmap->bind(2);
    }
    if (_bumpyShiny) {
        _bumpyShiny->bind(3);
    }
    if (_bumpmap) {
        _bumpmap->bind(4);
    }

    GLint blendSrcRgb, blendSrcAlpha, blendDstRgb, blendDstAlpha;
    if (additive) {
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRgb);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDstRgb);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDstAlpha);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    Mesh::renderTriangles();

    if (additive) {
        glBlendFuncSeparate(blendSrcRgb, blendDstRgb, blendSrcAlpha, blendDstAlpha);
    }
}

bool ModelMesh::shouldRender() const {
    return _render;
}

bool ModelMesh::isTransparent() const {
    if (_transparency > 0) return true;

    TextureFeatures features = _diffuse->features();
    if (features.blending == TextureBlending::Additive) return true;

    if (_envmap || _bumpyShiny || _bumpmap) return false;

    PixelFormat format = _diffuse->pixelFormat();
    if (format == PixelFormat::RGB || format == PixelFormat::BGR || format == PixelFormat::DXT1) return false;

    return true;
}

bool ModelMesh::hasDiffuseTexture() const {
    return static_cast<bool>(_diffuse);
}

bool ModelMesh::hasEnvmapTexture() const {
    return static_cast<bool>(_envmap);
}

bool ModelMesh::hasLightmapTexture() const {
    return static_cast<bool>(_lightmap);
}

bool ModelMesh::hasBumpyShinyTexture() const {
    return static_cast<bool>(_bumpyShiny);
}

bool ModelMesh::hasBumpmapTexture() const {
    return static_cast<bool>(_bumpmap);
}

int ModelMesh::transparency() const {
    return _transparency;
}

const shared_ptr<Texture> &ModelMesh::diffuseTexture() const {
    return _diffuse;
}

} // namespace render

} // namespace reone
