/*
 * Copyright © 2020 Vsevolod Kremianskii
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

ModelMesh::ModelMesh(bool render) : _render(render) {
}

void ModelMesh::render(const shared_ptr<Texture> &diffuseOverride) const {
    const shared_ptr<Texture> &diffuse = diffuseOverride ? diffuseOverride : _diffuse;
    bool additive = false;

    if (diffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuse->bind();
        additive = diffuse->isAdditive();
    }
    if (_envmap) {
        glActiveTexture(GL_TEXTURE1);
        _envmap->bind();
    }
    if (_lightmap) {
        glActiveTexture(GL_TEXTURE2);
        _lightmap->bind();
    }
    if (_bumpyShiny) {
        glActiveTexture(GL_TEXTURE3);
        _bumpyShiny->bind();
    }
    if (_bumpmap) {
        glActiveTexture(GL_TEXTURE4);
        _bumpmap->bind();
    }

    GLint blendSrcRgb, blendSrcAlpha, blendDstRgb, blendDstAlpha;
    if (additive) {
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRgb);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDstRgb);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDstAlpha);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    Mesh::render(GL_TRIANGLES);

    if (additive) {
        glBlendFuncSeparate(blendSrcRgb, blendDstRgb, blendSrcAlpha, blendDstAlpha);
    }

    if (_bumpmap) {
        glActiveTexture(GL_TEXTURE4);
        _bumpmap->unbind();
    }
    if (_bumpyShiny) {
        glActiveTexture(GL_TEXTURE3);
        _bumpyShiny->unbind();
    }
    if (_lightmap) {
        glActiveTexture(GL_TEXTURE2);
        _lightmap->unbind();
    }
    if (_envmap) {
        glActiveTexture(GL_TEXTURE1);
        _envmap->unbind();
    }
    if (diffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuse->unbind();
    }
}

bool ModelMesh::shouldRender() const {
    return _render;
}

bool ModelMesh::isTransparent() const {
    if (!_diffuse || _envmap) return false;

    PixelFormat format = _diffuse->pixelFormat();
    TextureFeatures features = _diffuse->features();

    return features.blending == TextureBlending::Additive || format == PixelFormat::DXT5 || format == PixelFormat::BGRA;
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

const shared_ptr<Texture> &ModelMesh::diffuseTexture() const {
    return _diffuse;
}

} // namespace render

} // namespace reone
