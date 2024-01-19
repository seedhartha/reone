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

#pragma once

#include "reone/graphics/modelnode.h"
#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

namespace reone {

class IInputStream;

namespace graphics {

class Animation;
class IStatistic;
class Model;

class MdlMdxReader : boost::noncopyable {
public:
    MdlMdxReader(IInputStream &mdl,
                 IInputStream &mdx,
                 IStatistic &statistic) :
        _mdl(BinaryReader(mdl)),
        _mdx(BinaryReader(mdx)),
        _statistic(statistic) {
    }

    void load();

    std::shared_ptr<Model> model() const { return _model; }

private:
    struct ArrayDefinition {
        uint32_t offset {0};
        uint32_t count {0};
        uint32_t count2 {0};
    };

    struct ControllerKey {
        uint32_t type {0};
        uint16_t numRows {0};
        uint16_t timeIndex {0};
        uint16_t dataIndex {0};
        uint8_t numColumns {0};
    };

    using MaterialMap = std::unordered_map<uint32_t, std::vector<uint32_t>>;

    BinaryReader _mdl;
    BinaryReader _mdx;
    IStatistic &_statistic;

    bool _tsl {false};
    std::vector<std::string> _nodeNames;
    std::vector<std::shared_ptr<ModelNode>> _nodes; /**< nodes in depth-first order */
    std::shared_ptr<Model> _model;
    std::string _modelName;
    uint32_t _offAnimRoot {0};

    ArrayDefinition readArrayDefinition();
    void readNodeNames(const std::vector<uint32_t> &offsets);
    std::shared_ptr<ModelNode> readNodes(uint32_t offset, ModelNode *parent, bool animated, bool animNode = false);
    std::vector<std::shared_ptr<Animation>> readAnimations(const std::vector<uint32_t> &offsets);
    std::unique_ptr<Animation> readAnimation(uint32_t offset);
    void readControllers(uint32_t keyOffset, uint32_t keyCount, const std::vector<float> &data, ModelNode &node);

    std::shared_ptr<ModelNode::Reference> readReference();
    std::shared_ptr<ModelNode::Light> readLight();
    std::shared_ptr<ModelNode::Emitter> readEmitter();
    std::shared_ptr<ModelNode::TriangleMesh> readMesh(int flags);

    std::shared_ptr<ModelNode::AABBTree> readAABBTree(uint32_t offset);

    void prepareSkinMeshes();

    // Controllers

    static void readFloatController(const ControllerKey &key,
                                    const std::vector<float> &data,
                                    ModelNode &node,
                                    KeyframeTrack<float> &track);

    static void readVectorController(const ControllerKey &key,
                                     const std::vector<float> &data,
                                     ModelNode &node,
                                     KeyframeTrack<glm::vec3> &track);

    static void readQuaternionController(const ControllerKey &key,
                                         const std::vector<float> &data,
                                         ModelNode &node,
                                         KeyframeTrack<glm::quat, SlerpInterpolator> &track);

    // END Controllers
};

} // namespace graphics

} // namespace reone
