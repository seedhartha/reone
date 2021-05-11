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

#include "aabbnode.h"
#include "model.h"

namespace reone {

namespace graphics {

class MdlReader : public resource::BinaryReader {
public:
    MdlReader();

    void load(const std::shared_ptr<std::istream> &mdl, const std::shared_ptr<std::istream> &mdx);

    std::shared_ptr<graphics::Model> model() const { return _model; }

private:
    struct ArrayDefinition {
        uint32_t offset { 0 };
        uint32_t count { 0 };
        uint32_t count2 { 0 };
    };

    struct MeshHeader {
        // Material
        glm::vec3 ambient { 0.0f };
        glm::vec3 diffuse { 0.0f };
        std::string texture1;
        std::string texture2;
        bool render { false };
        bool shadow { false };
        bool backgroundGeometry { false };
        int transparencyHint { 0 };

        // Geometry
        int numVertices { 0 };
        int numFaces { 0 };
        uint32_t offOffIndices { 0 };

        // MDX
        int mdxVertexSize { 0 };
        int offMdxVertices { 0 };
        int offMdxNormals { 0 };
        int offMdxTexCoords1 { 0 };
        int offMdxTexCoords2 { 0 };
        int offMdxTanSpace { 0 };

        // UV animation
        bool animateUV { false };
        float uvDirectionX { 0.0f };
        float uvDirectionY { 0.0f };
    };

    std::unique_ptr<StreamReader> _mdxReader;

    bool _tsl { false }; /**< is this a TSL model? */
    int _nodeIndex { 0 };
    std::vector<std::string> _nodeNames;
    std::unordered_map<uint32_t, int> _nodeFlags;
    bool _readingAnimations { false };

    std::shared_ptr<graphics::Model> _model;

    void doLoad() override;

    ArrayDefinition readArrayDefinition();
    void readNodeNames(const std::vector<uint32_t> &offsets);
    std::unique_ptr<graphics::ModelNode> readNode(uint32_t offset, graphics::ModelNode *parent);
    std::vector<std::shared_ptr<graphics::Animation>> readAnimations(const std::vector<uint32_t> &offsets);
    std::unique_ptr<graphics::Animation> readAnimation(uint32_t offset);

    void readControllers(int nodeFlags, uint32_t keyOffset, uint32_t keyCount, const std::vector<float> &data, graphics::ModelNode &node);
    void readLight(graphics::ModelNode &node);
    void readEmitter(graphics::ModelNode &node);
    void readReference(graphics::ModelNode &node);
    void readMesh(graphics::ModelNode &node);
    void readSkin(graphics::ModelNode &node);
    void readDanglymesh(graphics::ModelNode &node);
    void readAABB(graphics::ModelNode &node);
    void readSaber(graphics::ModelNode &node);

    void loadMesh(const MeshHeader &header, std::vector<float> &&vertices, std::vector<uint16_t> &&indices, VertexAttributes &&attributes, graphics::ModelNode &node);
    MeshHeader readMeshHeader();
    std::shared_ptr<AABBNode> readAABBNode(uint32_t offset);
};

} // namespace graphics

} // namespace reone
