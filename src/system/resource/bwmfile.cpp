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

#include "bwmfile.h"

#include "glm/ext.hpp"

using namespace std;

using namespace reone::render;

namespace reone {

namespace resource {

static vector<uint32_t> g_walkableTypes = { 1, 3, 4, 5, 9, 10 };

BwmFile::BwmFile() : BinaryFile(8, "BWM V1.0") {
}

void BwmFile::doLoad() {
    _type = readUint32();
    ignore(60);

    _vertexCount = readUint32();
    if (_vertexCount == 0) return;

    _vertexOffset = readUint32();
    _faceCount = readUint32();
    _faceOffset = readUint32();
    _faceTypeOffset = readUint32();

    loadVertices();
    loadFaces();
    loadFaceTypes();
    makeWalkmesh();
}

void BwmFile::loadVertices() {
    _vertices.reserve(3 * _vertexCount);
    seek(_vertexOffset);

    for (uint32_t i = 0; i < _vertexCount; ++i) {
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
    }
}

void BwmFile::loadFaces() {
    _indices.reserve(3 * _faceCount);
    seek(_faceOffset);

    for (uint32_t i = 0; i < _faceCount; ++i) {
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
    }
}

void BwmFile::loadFaceTypes() {
    _faceTypes.reserve(_faceCount);
    seek(_faceTypeOffset);

    for (uint32_t i = 0; i < _faceCount; ++i) {
        _faceTypes.push_back(readUint32());
    }
}

void BwmFile::makeWalkmesh() {
    _walkmesh = make_shared<Walkmesh>();
    _walkmesh->_vertices.reserve(_vertexCount);
    _walkmesh->_walkableFaces.reserve(_faceCount);

    for (uint32_t i = 0; i < 3 * _vertexCount; i += 3) {
        _walkmesh->_vertices.push_back(glm::make_vec3(&_vertices[i]));
    }

    for (uint32_t i = 0; i < _faceCount; ++i) {
        uint32_t type = _faceTypes[i];
        bool walkable = find(g_walkableTypes.begin(), g_walkableTypes.end(), type) != g_walkableTypes.end();

        int off = 3 * i;
        Walkmesh::Face face;
        face.type = type;
        face.indices.push_back(_indices[off + 0]);
        face.indices.push_back(_indices[off + 1]);
        face.indices.push_back(_indices[off + 2]);

        if (walkable) {
            _walkmesh->_walkableFaces.push_back(move(face));
        } else {
            _walkmesh->_nonWalkableFaces.push_back(move(face));
        }
    }

    _walkmesh->computeAABB();
}

shared_ptr<render::Walkmesh> BwmFile::walkmesh() const {
    return _walkmesh;
}

} // namespace resource

} // namespace reone

