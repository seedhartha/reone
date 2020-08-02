#include "bwmfile.h"

#include "glm/ext.hpp"

using namespace reone::render;

namespace reone {

namespace resources {

static std::vector<uint32_t> g_walkableTypes = { 1, 3, 4, 5, 9, 10 };

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

    for (int i = 0; i < _vertexCount; ++i) {
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
        _vertices.push_back(readFloat());
    }
}

void BwmFile::loadFaces() {
    _indices.reserve(3 * _faceCount);
    seek(_faceOffset);

    for (int i = 0; i < _faceCount; ++i) {
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
        _indices.push_back(readUint32());
    }
}

void BwmFile::loadFaceTypes() {
    _faceTypes.reserve(_faceCount);
    seek(_faceTypeOffset);

    for (int i = 0; i < _faceCount; ++i) {
        _faceTypes.push_back(readUint32());
    }
}

void BwmFile::makeWalkmesh() {
    _walkmesh = std::make_shared<Walkmesh>();
    _walkmesh->_vertices.reserve(_vertexCount);
    _walkmesh->_walkableFaces.reserve(_faceCount);

    for (int i = 0; i < 3 * _vertexCount; i += 3) {
        _walkmesh->_vertices.push_back(glm::make_vec3(&_vertices[i]));
    }

    for (int i = 0; i < _faceCount; ++i) {
        uint32_t type = _faceTypes[i];
        bool walkable = std::find(g_walkableTypes.begin(), g_walkableTypes.end(), type) != g_walkableTypes.end();

        int off = 3 * i;
        Walkmesh::Face face;
        face.type = type;
        face.indices.push_back(_indices[off + 0]);
        face.indices.push_back(_indices[off + 1]);
        face.indices.push_back(_indices[off + 2]);

        if (walkable) {
            _walkmesh->_walkableFaces.push_back(std::move(face));
        } else {
            _walkmesh->_nonWalkableFaces.push_back(std::move(face));
        }
    }

    _walkmesh->computeAABB();
}

std::shared_ptr<render::Walkmesh> BwmFile::walkmesh() const {
    return _walkmesh;
}

} // namespace resources

} // namespace reone

