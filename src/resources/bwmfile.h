#pragma once

#include "binfile.h"

#include "../render/walkmesh.h"

namespace reone {

namespace resources {

class BwmFile : public BinaryFile {
public:
    BwmFile();
    std::shared_ptr<render::Walkmesh> walkmesh() const;

private:
    uint32_t _type { 0 };
    uint32_t _vertexCount { 0 };
    uint32_t _vertexOffset { 0 };
    uint32_t _faceCount { 0 };
    uint32_t _faceOffset { 0 };
    uint32_t _faceTypeOffset { 0 };
    std::vector<float> _vertices;
    std::vector<uint32_t> _indices;
    std::vector<uint32_t> _faceTypes;
    std::shared_ptr<render::Walkmesh> _walkmesh;

    void doLoad() override;
    void loadVertices();
    void loadFaces();
    void loadFaceTypes();
    void makeWalkmesh();
};

} // namespace resources

} // namespace reone
