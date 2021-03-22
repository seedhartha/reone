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

#include "../../resource/format/binreader.h"
#include "../../resource/types.h"

#include "../model/model.h"
#include "../model/modelloader.h"

namespace reone {

namespace render {

class MdlReader : public resource::BinaryReader {
public:
    struct ControllerKey {
        uint32_t type { 0 };
        uint16_t unknown1 { 0 };
        uint16_t rowCount { 0 };
        uint16_t timeIndex { 0 };
        uint16_t dataIndex { 0 };
        uint8_t columnCount { 0 };
    };

    MdlReader(resource::GameID gameId);

    void load(const std::shared_ptr<std::istream> &mdl, const std::shared_ptr<std::istream> &mdx);

    std::shared_ptr<render::Model> model() const { return _model; }

private:
    resource::GameID _gameId;

    std::shared_ptr<std::istream> _mdx;
    std::unique_ptr<StreamReader> _mdxReader;
    std::string _name;
    Model::Classification _classification { Model::Classification::Other };
    int _nodeIndex { 0 };
    std::vector<std::string> _nodeNames;
    std::shared_ptr<render::Model> _model;

    void doLoad() override;

    std::unique_ptr<render::Animation> readAnimation(uint32_t offset);
    std::unique_ptr<render::ModelMesh> readMesh(const std::string &nodeName, int nodeFlags);
    std::unique_ptr<render::ModelNode> readNode(uint32_t offset, render::ModelNode *parent);
    std::vector<std::shared_ptr<render::Animation>> readAnimations(const std::vector<uint32_t> &offsets);
    void readArrayDefinition(uint32_t &offset, uint32_t &count);
    void readControllers(uint32_t keyCount, uint32_t keyOffset, const std::vector<float> &data, render::ModelNode &node);
    void readEmitter(render::ModelNode &node);
    void readLight(render::ModelNode &node);
    void readNodeNames(const std::vector<uint32_t> &offsets);
    void readSkin(render::ModelNode &node);
    void readReference(render::ModelNode &node);

    Model::Classification getClassification(int value) const;
};

class MdlModelLoader : public IModelLoader {
public:
    std::shared_ptr<Model> loadModel(resource::GameID gameId, const std::string &resRef) override;
};

} // namespace render

} // namespace reone
