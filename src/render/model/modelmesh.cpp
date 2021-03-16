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

using namespace std;

namespace reone {

namespace render {

ModelMesh::ModelMesh(const shared_ptr<Mesh> &mesh) : _mesh(mesh) {
    if (!mesh) {
        throw invalid_argument("mesh must not be null");
    }
}

void ModelMesh::initGL() {
    _mesh->init();
}

void ModelMesh::deinitGL() {
    _mesh->deinit();
}

void ModelMesh::draw() {
    _mesh->draw();
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

void ModelMesh::setDiffuseTexture(shared_ptr<Texture> texture) {
    _diffuse = move(texture);
}

void ModelMesh::setBumpmapTexture(shared_ptr<Texture> texture, bool swizzled) {
    _bumpmap = move(texture);
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
