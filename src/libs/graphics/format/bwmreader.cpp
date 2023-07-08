/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/format/bwmreader.h"

#include "reone/graphics/walkmesh.h"
#include "reone/resource/format/signutil.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

void BwmReader::load() {
    checkSignature(_bwm, std::string("BWM V1.0", 8));

    _type = static_cast<WalkmeshType>(_bwm.readUint32());

    std::vector<float> relUsePosition1(_bwm.readFloatArray(3));
    std::vector<float> relUsePosition2(_bwm.readFloatArray(3));
    std::vector<float> absUsePosition1(_bwm.readFloatArray(3));
    std::vector<float> absUsePosition2(_bwm.readFloatArray(3));

    std::vector<float> position(_bwm.readFloatArray(3));
    _position = glm::make_vec3(&position[0]);

    _numVertices = _bwm.readUint32();
    if (_numVertices == 0) {
        return;
    }

    _offVertices = _bwm.readUint32();
    _numFaces = _bwm.readUint32();
    _offIndices = _bwm.readUint32();
    _offMaterials = _bwm.readUint32();
    _offNormals = _bwm.readUint32();
    _offPlanarDistances = _bwm.readUint32();

    if (_type == WalkmeshType::WOK) {
        _numAabb = _bwm.readUint32();
        _offAabb = _bwm.readUint32();

        _bwm.ignore(4); // unknown

        _numAdjacencies = _bwm.readUint32();
        _offAdjacencies = _bwm.readUint32();
        _numEdges = _bwm.readUint32();
        _offsetEdges = _bwm.readUint32();
        _numPerimeters = _bwm.readUint32();
        _offPerimeters = _bwm.readUint32();
    }

    _walkmesh = std::make_shared<Walkmesh>();
    _walkmesh->_area = _type == WalkmeshType::WOK;

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

        _walkmesh->_faces.push_back(std::move(face));
    }

    if (_type == WalkmeshType::WOK) {
        loadAABB();
    }
}

void BwmReader::loadVertices() {
    _bwm.seek(_offVertices);
    _vertices.reserve(3 * _numVertices);

    for (uint32_t i = 0; i < _numVertices; ++i) {
        _vertices.push_back(_bwm.readFloat());
        _vertices.push_back(_bwm.readFloat());
        _vertices.push_back(_bwm.readFloat());
    }
}

void BwmReader::loadIndices() {
    _bwm.seek(_offIndices);
    _indices.reserve(3 * _numFaces);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _indices.push_back(_bwm.readUint32());
        _indices.push_back(_bwm.readUint32());
        _indices.push_back(_bwm.readUint32());
    }
}

void BwmReader::loadMaterials() {
    _bwm.seek(_offMaterials);
    _materials.reserve(_numFaces);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _materials.push_back(_bwm.readUint32());
    }
}

void BwmReader::loadNormals() {
    _bwm.seek(_offNormals);
    _normals.reserve(3 * _numFaces);

    for (uint32_t i = 0; i < _numFaces; ++i) {
        _normals.push_back(_bwm.readFloat());
        _normals.push_back(_bwm.readFloat());
        _normals.push_back(_bwm.readFloat());
    }
}

void BwmReader::loadAABB() {
    _bwm.seek(_offAabb);

    std::vector<std::shared_ptr<Walkmesh::AABB>> aabbs;
    aabbs.resize(_numAabb);

    std::vector<std::pair<uint32_t, uint32_t>> aabbChildren;
    aabbChildren.resize(_numAabb);

    for (uint32_t i = 0; i < _numAabb; ++i) {
        std::vector<float> bounds(_bwm.readFloatArray(6));
        int faceIdx = _bwm.readInt32();
        _bwm.ignore(4); // unknown
        uint32_t mostSignificantPlane = _bwm.readUint32();
        uint32_t childIdx1 = _bwm.readUint32();
        uint32_t childIdx2 = _bwm.readUint32();

        aabbs[i] = std::make_shared<Walkmesh::AABB>();
        aabbs[i]->value = AABB(glm::make_vec3(&bounds[0]), glm::make_vec3(&bounds[3]));
        aabbs[i]->faceIdx = faceIdx;

        aabbChildren[i] = std::make_pair(childIdx1, childIdx2);
    }

    for (uint32_t i = 0; i < _numAabb; ++i) {
        if (aabbs[i]->faceIdx != -1) {
            continue;
        }
        uint32_t childIdx1 = aabbChildren[i].first;
        uint32_t childIdx2 = aabbChildren[i].second;
        aabbs[i]->left = aabbs[childIdx1];
        aabbs[i]->right = aabbs[childIdx2];
    }

    _walkmesh->_rootAabb = aabbs[0];
}

} // namespace graphics

} // namespace reone
