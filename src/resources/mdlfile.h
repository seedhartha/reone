#pragma once

#include "../render/model.h"

#include "binfile.h"
#include "types.h"

namespace reone {

namespace resources {

class MdlFile : public BinaryFile {
public:
    MdlFile(GameVersion version);

    void load(const std::shared_ptr<std::istream> &mdl, const std::shared_ptr<std::istream> &mdx);
    std::shared_ptr<render::Model> model() const;

private:
    GameVersion _version { GameVersion::KotOR };
    std::shared_ptr<std::istream> _mdx;
    std::string _name;
    int _nodeIndex { 0 };
    std::vector<std::string> _nodeNames;
    std::shared_ptr<render::Model> _model;

    void doLoad() override;
    void openMDX();
    void readArrayDefinition(uint32_t &offset, uint32_t &count);
    void readNodeNames(const std::vector<uint32_t> &offsets);
    std::unique_ptr<render::ModelNode> readNode(uint32_t offset, render::ModelNode *parent);
    void readControllers(uint32_t keyCount, uint32_t keyOffset, const std::vector<float> &data, render::ModelNode &node);
    void readPositionController(uint16_t rowCount, uint8_t columnCount, uint16_t timeIndex, uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readOrientationController(uint16_t rowCount, uint8_t columnCount, uint16_t timeIndex, uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readAlphaController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    std::unique_ptr<render::ModelMesh> readMesh();
    void readSkin(render::ModelNode &node);
    std::vector<std::shared_ptr<render::Animation>> readAnimations(const std::vector<uint32_t> &offsets);
    std::unique_ptr<render::Animation> readAnimation(uint32_t offset);
};

} // namespace resources

} // namespace reone
