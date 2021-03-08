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

#include "bwmfile.h"

#include "glm/ext.hpp"

#include "walkmeshutil.h"

using namespace std;

namespace reone {

namespace render {

BwmFile::BwmFile() : BinaryFile(8, "BWM V1.0") {
}

void BwmFile::doLoad() {
    _type = static_cast<WalkmeshType>(readUint32());

    ignore(48 + 12); // reserved + position

    _numVertices = readUint32();
    if (_numVertices == 0) return;

    _offsetVertices = readUint32();
    _numFaces = readUint32();
    _offsetIndices = readUint32();
    _offsetMaterials = readUint32();
    _offsetNormals = readUint32();
    _offsetPlanarDistances = readUint32();

    if (_type == WalkmeshType::WOK) {
        _numAabb = readUint32();
        _offsetAabb = readUint32();

        ignore(4); // unknown

        _numAdjacencies = readUint32();
        _offsetAdjacencies = readUint32();
        _numEdges = readUint32();
        _offsetEdges = readUint32();
        _numPerimeters = readUint32();
        _offsetPerimeters = readUint32();
    }

    loadVertices();
    loadIndices();
    loadMaterials();
    loadNormals();

    makeWalkmesh();
}

void BwmFile::loadVertices() {
    _vertices.reserve(3 * _numVertices);
    seek(_offsetVertices);

    for (uint32_t i = 0; i < _numVertices; ++i) {
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
    }
}

void BwmFile::loadIndices() {
    _indices.reserve(3 * _numFaces);
    seek(_offsetIndices);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
    }
}

void BwmFile::loadMaterials() {
    _materials.reserve(_numFaces);
    seek(_offsetMaterials);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        auto material = static_cast<WalkmeshMaterial>(readUint32());
        _materials.push_back(material);
    }
}

void BwmFile::loadNormals() {
    _normals.reserve(3 * _numFaces);
    seek(_offsetNormals);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _normals.push_back(readFloat());
        _normals.push_back(readFloat());
        _normals.push_back(readFloat());
    }
}

void BwmFile::makeWalkmesh() {
    _walkmesh = make_shared<Walkmesh>();

    for (uint32_t i = 0; i < _numFaces; ++i) {
        WalkmeshMaterial material = _materials[i];
        bool walkable = isMaterialWalkable(material);

        uint32_t *indices = &_indices[3 * i + 0];

        Walkmesh::Face face;
        face.material = material;
        face.vertices.push_back(glm::make_vec3(&_vertices[3 * indices[0]]));
        face.vertices.push_back(glm::make_vec3(&_vertices[3 * indices[1]]));
        face.vertices.push_back(glm::make_vec3(&_vertices[3 * indices[2]]));
        face.normal = glm::make_vec3(&_normals[3 * i]);

        if (walkable) {
            _walkmesh->_walkableFaces.push_back(move(face));
        } else {
            _walkmesh->_nonWalkableFaces.push_back(move(face));
        }
    }

    _walkmesh->computeAABB();
}

} // namespace render

} // namespace reone

