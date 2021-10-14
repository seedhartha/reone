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

#include "../../../common/streamwriter.h"

#include "../modelnode.h"

namespace reone {

namespace graphics {

class Model;

class MdlWriter {
public:
    MdlWriter(Model &model, bool tsl) :
        _model(model),
        _tsl(tsl) {
    }

    void save(const boost::filesystem::path &mdlPath, const boost::filesystem::path &mdxPath);

private:
    Model &_model;
    bool _tsl;

    void writeFileHeader(uint32_t mdlSize, uint32_t mdxSize, StreamWriter &mdl);
    void writeGeometryHeader(int numNodes, uint32_t offRootNode, StreamWriter &mdl);
    void writeModelHeader(int numNodes, uint32_t offRootNode, uint32_t mdxSize, StreamWriter &mdl);

    void writeNodeNameOffsets(const std::vector<uint32_t> &offsets, StreamWriter &mdl);
    void writeNodeNames(ModelNode &node, StreamWriter &mdl, std::vector<uint32_t> &offsets);
    void writeNodes(uint32_t offRoot, uint32_t offParent, uint32_t offNode, uint32_t offRaw, ModelNode &node, StreamWriter &mdl, StreamWriter &raw, StreamWriter &mdx, int &dfsIdx);
    void writeMesh(uint32_t offRaw, const ModelNode::TriangleMesh &mesh, StreamWriter &mdl, StreamWriter &raw, StreamWriter &mdx);

    void writeArrayDef(uint32_t offset, uint32_t count, StreamWriter &mdl);

    uint32_t getTreeSize(const ModelNode &node) const;
    uint32_t getNodeSize(const ModelNode &node) const;
};

} // namespace graphics

} // namespace reone
