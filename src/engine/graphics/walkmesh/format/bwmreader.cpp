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

#include "bwmreader.h"

#include "../walkmesh.h"

using namespace std;

namespace reone {

namespace graphics {

BwmReader::BwmReader(set<uint32_t> walkableSurfaces) :
    BinaryReader(8, "BWM V1.0"),
    _walkableSurfaces(move(walkableSurfaces)) {
}

void BwmReader::doLoad() {
    _type = static_cast<WalkmeshType>(readUint32());

    ignore(48 + 12); // reserved + position

    _numVertices = readUint32();
    if (_numVertices == 0)
        return;

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

    _walkmesh = make_shared<Walkmesh>();

    loadVertices();
    loadIndices();
    loadMaterials();
    loadNormals();

    for (uint32_t i = 0; i < _numFaces; ++i) {
        uint32_t material = _materials[i];
        uint32_t *indices = &_indices[3 * i + 0];

        Walkmesh::Face face;
        face.index = i;
        face.material = material;
        face.vertices.push_back(glm::make_vec3(&_vertices[3 * indices[0]]));
        face.vertices.push_back(glm::make_vec3(&_vertices[3 * indices[1]]));
        face.vertices.push_back(glm::make_vec3(&_vertices[3 * indices[2]]));
        face.normal = glm::make_vec3(&_normals[3 * i]);

        bool walkable = _walkableSurfaces.count(material) > 0;
        if (walkable) {
            _walkmesh->_walkableFaces.push_back(move(face));
        } else {
            _walkmesh->_nonWalkableFaces.push_back(move(face));
        }
    }

    if (_type == WalkmeshType::WOK) {
        loadAABB();
    }
}

void BwmReader::loadVertices() {
    seek(_offsetVertices);
    _vertices.reserve(3 * _numVertices);

    for (uint32_t i = 0; i < _numVertices; ++i) {
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
    }
}

void BwmReader::loadIndices() {
    seek(_offsetIndices);
    _indices.reserve(3 * _numFaces);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
    }
}

void BwmReader::loadMaterials() {
    seek(_offsetMaterials);
    _materials.reserve(_numFaces);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _materials.push_back(readUint32());
    }
}

void BwmReader::loadNormals() {
    seek(_offsetNormals);
    _normals.reserve(3 * _numFaces);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _normals.push_back(readFloat());
        _normals.push_back(readFloat());
        _normals.push_back(readFloat());
    }
}

void BwmReader::loadAABB() {
    seek(_offsetAabb);

    vector<shared_ptr<Walkmesh::AABB>> aabbs;
    aabbs.resize(_numAabb);

    vector<pair<uint32_t, uint32_t>> aabbChildren;
    aabbChildren.resize(_numAabb);

    for (uint32_t i = 0; i < _numAabb; ++i) {
        vector<float> bounds(readFloatArray(6));
        int faceIdx = readInt32();
        ignore(4); // unknown
        uint32_t mostSignificantPlane = readUint32();
        uint32_t childIdx1 = readUint32();
        uint32_t childIdx2 = readUint32();

        aabbs[i] = make_shared<Walkmesh::AABB>();
        aabbs[i]->min = glm::make_vec3(&bounds[0]);
        aabbs[i]->max = glm::make_vec3(&bounds[3]);
        aabbs[i]->faceIdx = faceIdx;

        aabbChildren[i] = make_pair(childIdx1, childIdx2);
    }

    for (uint32_t i = 0; i < _numAabb; ++i) {
        if (aabbs[i]->faceIdx != -1) {
            continue;
        }
        uint32_t childIdx1 = aabbChildren[i].first;
        uint32_t childIdx2 = aabbChildren[i].second;
        aabbs[i]->child1 = aabbs[childIdx1];
        aabbs[i]->child2 = aabbs[childIdx2];
    }

    _walkmesh->_rootAabb = aabbs[0];
}

} // namespace graphics

} // namespace reone
