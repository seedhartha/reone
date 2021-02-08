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

#include "modelmesh.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../stateutil.h"

using namespace std;

namespace reone {

namespace render {

ModelMesh::ModelMesh(const shared_ptr<Mesh> &mesh) : _mesh(mesh) {
    if (!mesh) {
        throw invalid_argument("mesh must not be null");
    }
}

void ModelMesh::initGL() {
    _mesh->initGL();
}

void ModelMesh::deinitGL() {
    _mesh->deinitGL();
}

void ModelMesh::render(const shared_ptr<Texture> &diffuseOverride) const {
    const shared_ptr<Texture> &diffuse = diffuseOverride ? diffuseOverride : _diffuse;
    bool additive = false;

    if (diffuse) {
        setActiveTextureUnit(0);
        diffuse->bind();
        additive = diffuse->isAdditive();
    }
    if (_envmap) {
        setActiveTextureUnit(1);
        _envmap->bind();
    }
    if (_lightmap) {
        setActiveTextureUnit(2);
        _lightmap->bind();
    }
    if (_bumpyShiny) {
        setActiveTextureUnit(3);
        _bumpyShiny->bind();
    }
    if (_bumpmap) {
        setActiveTextureUnit(4);
        _bumpmap->bind();
    }

    if (additive) {
        withAdditiveBlending([this]() { _mesh->renderTriangles(); });
    } else {
        _mesh->renderTriangles();
    }
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

void ModelMesh::setRender(bool render) {
    _render = render;
}

void ModelMesh::setTransparency(int transparency) {
    _transparency = transparency;
}

void ModelMesh::setShadow(bool shadow) {
    _shadow = shadow;
}

void ModelMesh::setBackgroundGeometry(bool background) {
    _backgroundGeometry = background;
}

void ModelMesh::setDiffuseTexture(const shared_ptr<Texture> &texture) {
    _diffuse = texture;
}

void ModelMesh::setBumpmapTexture(const shared_ptr<Texture> &texture, bool swizzled) {
    _bumpmap = texture;
    _bumpmapSwizzled = swizzled;
}

void ModelMesh::setDiffuseColor(glm::vec3 color) {
    _diffuseColor = move(color);
}

void ModelMesh::setAmbientColor(glm::vec3 color) {
    _ambientColor = move(color);
}

} // namespace render

} // namespace reone
