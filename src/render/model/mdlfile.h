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

#pragma once

#include "../../resource/format/binfile.h"
#include "../../resource/types.h"

#include "../model/model.h"

namespace reone {

namespace render {

class MdlFile : public resource::BinaryFile {
public:
    MdlFile(resource::GameVersion version);

    void load(const std::shared_ptr<std::istream> &mdl, const std::shared_ptr<std::istream> &mdx);
    std::shared_ptr<render::Model> model() const;

private:
    resource::GameVersion _version { resource::GameVersion::KotOR };
    std::shared_ptr<std::istream> _mdx;
    std::unique_ptr<StreamReader> _mdxReader;
    std::string _name;
    int _nodeIndex { 0 };
    std::vector<std::string> _nodeNames;
    std::shared_ptr<render::Model> _model;

    void doLoad() override;
    void openMDX();

    std::unique_ptr<render::Animation> readAnimation(uint32_t offset);
    std::unique_ptr<render::ModelMesh> readMesh(const std::string &nodeName, int nodeFlags);
    std::unique_ptr<render::ModelNode> readNode(uint32_t offset, render::ModelNode *parent);
    std::vector<std::unique_ptr<render::Animation>> readAnimations(const std::vector<uint32_t> &offsets);
    void readArrayDefinition(uint32_t &offset, uint32_t &count);
    void readControllers(uint32_t keyCount, uint32_t keyOffset, const std::vector<float> &data, render::ModelNode &node);
    void readEmitter(render::ModelNode &node);
    void readLight(render::ModelNode &node);
    void readNodeNames(const std::vector<uint32_t> &offsets);
    void readSkin(render::ModelNode &node);

    Model::Classification getClassification(int value) const;

    // Controllers

    void readAlphaController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readAlphaEndController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readAlphaMidController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readAlphaStartController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readBirthrateController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readColorController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readColorEndController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readColorMidController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readColorStartController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readFPSController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readFrameEndController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readFrameStartController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readLifeExpectancyController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readMultiplierController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readOrientationController(uint16_t rowCount, uint8_t columnCount, uint16_t timeIndex, uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readPositionController(uint16_t rowCount, uint8_t columnCount, uint16_t timeIndex, uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readRadiusController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readRandomVelocityController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readScaleController(uint16_t rowCount, uint16_t timeIndex, uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSelfIllumColorController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSizeEndController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSizeMidController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSizeStartController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSizeXController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSizeYController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readSpreadController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);
    void readVelocityController(uint16_t dataIndex, const std::vector<float> &data, render::ModelNode &node);

    // END Controllers
};

} // namespace render

} // namespace reone
