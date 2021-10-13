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

#include "mdlwriter.h"

#include "../../mesh/mesh.h"
#include "../../types.h"

#include "../model.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace graphics {

static constexpr int kGeometryHeaderSize = 80;
static constexpr int kModelHeaderSize = 116;
static constexpr int kNodeSizeDummy = 80;
static constexpr int kNodeSizeTrimeshKotor = 332;
static constexpr int kNodeSizeTrimeshTsl = 340;

static constexpr float kDefaultRadius = 7.0f;

static const vector<float> g_defaultBoundingBox {-5.0f, -5.0f, -1.0f, 5.0f, 5.0f, 10.0f};

void MdlWriter::save(const fs::path &mdlPath, const fs::path &mdxPath) {
    // Write to MDL/MDX buffers

    auto mdxBuf = make_shared<stringstream>();
    StreamWriter mdxBufWriter(mdxBuf);

    auto nodeNamesBuf = make_shared<stringstream>();
    StreamWriter nodeNamesWriter(nodeNamesBuf);
    vector<uint32_t> nodeNameOffsets;
    writeNodeNames(*_model.rootNode(), nodeNamesWriter, nodeNameOffsets);

    string nodeNamesData(nodeNamesBuf->str());
    uint32_t nodeNamesSize = nodeNamesData.length();
    int numNodes = static_cast<int>(nodeNameOffsets.size());
    uint32_t offNodes = kGeometryHeaderSize + kModelHeaderSize + 4 * numNodes + nodeNamesSize;

    auto nodesBuf = make_shared<stringstream>();
    StreamWriter nodesWriter(nodesBuf);
    int nodeDfsIdx = 0;
    writeNodes(offNodes, 0, *_model.rootNode(), nodesWriter, mdxBufWriter, nodeDfsIdx);

    // TODO: animations

    auto mdlBuf = make_shared<stringstream>();
    StreamWriter mdlBufWriter(mdlBuf);

    writeGeometryHeader(numNodes, offNodes, mdlBufWriter);
    writeModelHeader(numNodes, offNodes, mdlBufWriter);
    writeNodeNameOffsets(nodeNameOffsets, mdlBufWriter);
    mdlBufWriter.putString(nodeNamesData);
    mdlBufWriter.putString(nodesBuf->str());

    // END Write to MDL/MDX buffers

    // Flush MDL/MDX buffers

    string mdlData(mdlBuf->str());
    uint32_t mdlSize = static_cast<uint32_t>(mdlData.length());

    string mdxData(mdxBuf->str());
    uint32_t mdxSize = static_cast<uint32_t>(mdxData.length());

    auto mdl = make_shared<fs::ofstream>(mdlPath, ios::binary);
    StreamWriter mdlWriter(mdl);
    writeFileHeader(mdlSize, mdxSize, mdlWriter);
    mdlWriter.putString(mdlData);

    if (mdxSize > 0) {
        auto mdx = make_shared<fs::ofstream>(mdxPath, ios::binary);
        StreamWriter mdxWriter(mdx);
        mdxWriter.putString(mdxData);
    }

    // END Flush MDL/MDX buffers
}

void MdlWriter::writeFileHeader(uint32_t mdlSize, uint32_t mdxSize, StreamWriter &mdl) {
    mdl.putUint32(0); // pseudo signature
    mdl.putUint32(mdlSize);
    mdl.putUint32(mdxSize);
}

void MdlWriter::writeGeometryHeader(int numNodes, uint32_t offRootNode, StreamWriter &mdl) {
    uint32_t funcPtr1 = _tsl ? kMdlModelFuncPtr1TslPC : kMdlModelFuncPtr1KotorPC;
    uint32_t funcPtr2 = _tsl ? kMdlModelFuncPtr2TslPC : kMdlModelFuncPtr2KotorPC;
    uint32_t refCount = 0;
    uint8_t modelType = 2;

    mdl.putUint32(funcPtr1);
    mdl.putUint32(funcPtr2);
    mdl.putStringExact(_model.name(), 32);
    mdl.putUint32(offRootNode);
    mdl.putUint32(static_cast<uint32_t>(numNodes));
    mdl.putBytes(6 * 4, 0); // unknown
    mdl.putUint32(refCount);
    mdl.putByte(modelType);
    mdl.putBytes(3, 0); // padding
}

void MdlWriter::writeModelHeader(int numNodes, uint32_t offRootNode, StreamWriter &mdl) {
    uint8_t subclassification = 0;
    uint8_t affectedByFog = 1;
    uint32_t numChildModels = 0;
    uint32_t superModelRef = 0;
    uint32_t mdxSize2 = 0;
    uint32_t mdxOffset = 0;

    mdl.putByte(static_cast<uint8_t>(_model.classification()));
    mdl.putByte(subclassification);
    mdl.putByte(0); // unknown
    mdl.putByte(affectedByFog);
    mdl.putUint32(numChildModels);
    writeArrayDef(offRootNode, 0, mdl); // animations
    mdl.putUint32(superModelRef);
    for (auto &val : g_defaultBoundingBox) {
        mdl.putFloat(val);
    }
    mdl.putFloat(kDefaultRadius);
    mdl.putFloat(_model.animationScale());
    mdl.putStringExact("NULL", 32);
    mdl.putUint32(offRootNode);
    mdl.putUint32(0); // unknown
    mdl.putUint32(mdxSize2);
    mdl.putUint32(mdxOffset);
    writeArrayDef(kGeometryHeaderSize + kModelHeaderSize, numNodes, mdl); // node names
}

void MdlWriter::writeNodeNameOffsets(const vector<uint32_t> &offsets, StreamWriter &mdl) {
    size_t start = mdl.tell() + 4 * offsets.size();

    for (auto &offset : offsets) {
        mdl.putUint32(start + offset);
    }
}

void MdlWriter::writeNodeNames(ModelNode &node, StreamWriter &mdl, vector<uint32_t> &offsets) {
    offsets.push_back(static_cast<uint32_t>(mdl.tell()));
    mdl.putCString(node.name());

    for (auto &child : node.children()) {
        writeNodeNames(*child, mdl, offsets);
    }
}

void MdlWriter::writeNodes(uint32_t offNodes, uint32_t offParent, ModelNode &node, StreamWriter &mdl, StreamWriter &mdx, int &dfsIdx) {
    uint32_t offThis = offNodes + static_cast<uint32_t>(mdl.tell());

    uint32_t nodeSize = kNodeSizeDummy;
    if (node.flags() & MdlNodeFlags::mesh) {
        nodeSize += _tsl ? kNodeSizeTrimeshTsl : kNodeSizeTrimeshKotor;
    }

    uint16_t flags = node.flags();
    uint16_t nodeId = dfsIdx;
    uint16_t nameIndex = dfsIdx;
    uint32_t offRootNode = offNodes;
    uint32_t offParentNode = offParent;
    uint32_t offChildren = offThis + nodeSize;
    uint32_t numChildren = static_cast<uint32_t>(node.children().size());

    mdl.putUint16(flags);
    mdl.putUint16(nodeId);
    mdl.putUint16(nameIndex);
    mdl.putUint16(0); // padding
    mdl.putUint32(offRootNode);
    mdl.putUint32(offParentNode);
    for (int i = 0; i < 3; ++i) {
        mdl.putFloat(node.restPosition()[i]);
    }
    mdl.putFloat(node.restOrientation().w);
    mdl.putFloat(node.restOrientation().x);
    mdl.putFloat(node.restOrientation().y);
    mdl.putFloat(node.restOrientation().z);
    writeArrayDef(offChildren, numChildren, mdl); // children
    writeArrayDef(0, 0, mdl);                     // controllers
    writeArrayDef(0, 0, mdl);                     // controllers data

    if (node.flags() & MdlNodeFlags::mesh) {
        writeMesh(*node.mesh(), mdl, mdx);
    }
    for (auto &child : node.children()) {
        writeNodes(offNodes, offThis, *child, mdl, mdx, ++dfsIdx);
    }
}

void MdlWriter::writeMesh(const ModelNode::TriangleMesh &mesh, StreamWriter &mdl, StreamWriter &mdx) {
    uint32_t funcPtr1 = _tsl ? kMdlMeshFuncPtr1TslPC : kMdlMeshFuncPtr1KotorPC;
    uint32_t funcPtr2 = _tsl ? kMdlMeshFuncPtr2TslPC : kMdlMeshFuncPtr2KotorPC;
    vector<float> average {0.0f, 0.0f, 0.0f};
    uint32_t transparencyHint = 0;
    uint32_t animateUV = 0;
    float uvDirectionX = 0.0f;
    float uvDirectionY = 0.0f;
    float uvJitter = 0.0f;
    float uvJitterSpeed = 0.0f;
    uint32_t mdxDataFlags = 0;
    int offMdxVertexColors = 0;
    int offMdxTexCoords3 = 0;
    int offMdxTexCoords4 = 0;
    int offMdxTanSpace = 0;
    uint16_t numVertices = static_cast<uint16_t>(mesh.mesh->vertices().size() / (mesh.mesh->attributes().stride / sizeof(float)));
    uint16_t numTextures = 0;
    uint8_t lightmapped = 0;
    uint8_t rotateTexture = 0;
    uint8_t backgroundGeometry = 0;
    uint8_t shadow = 0;
    uint8_t beaming = 0;
    float totalArea = 0.0f;
    uint32_t offMdxData = static_cast<uint32_t>(mdx.tell());
    uint32_t offVertices = 0;

    mdl.putUint32(funcPtr1);
    mdl.putUint32(funcPtr2);
    writeArrayDef(0, 0, mdl); // faces
    for (auto &val : g_defaultBoundingBox) {
        mdl.putFloat(val);
    }
    mdl.putFloat(kDefaultRadius);
    for (auto &val : average) {
        mdl.putFloat(val);
    }
    for (int i = 0; i < 3; ++i) {
        mdl.putFloat(mesh.diffuse[i]);
    }
    for (int i = 0; i < 3; ++i) {
        mdl.putFloat(mesh.ambient[i]);
    }
    mdl.putUint32(transparencyHint);
    mdl.putStringExact("NULL", 32);
    mdl.putStringExact("NULL", 32);
    mdl.putStringExact("NULL", 12);
    mdl.putStringExact("NULL", 12);
    writeArrayDef(0, 0, mdl);   // indices count
    writeArrayDef(0, 0, mdl);   // indices offset
    writeArrayDef(0, 0, mdl);   // inverted counter
    mdl.putBytes(3 * 4 + 8, 0); // unknown
    mdl.putUint32(animateUV);
    mdl.putFloat(uvDirectionX);
    mdl.putFloat(uvDirectionY);
    mdl.putFloat(uvJitter);
    mdl.putFloat(uvJitterSpeed);
    mdl.putUint32(mesh.mesh->attributes().stride);
    mdl.putUint32(mdxDataFlags);
    mdl.putInt32(mesh.mesh->attributes().offCoords);
    mdl.putInt32(mesh.mesh->attributes().offNormals);
    mdl.putInt32(offMdxVertexColors);
    mdl.putInt32(mesh.mesh->attributes().offTexCoords1);
    mdl.putInt32(mesh.mesh->attributes().offTexCoords2);
    mdl.putInt32(offMdxTexCoords3);
    mdl.putInt32(offMdxTexCoords4);
    mdl.putInt32(offMdxTanSpace);
    mdl.putBytes(3 * 4, 0); // unknown
    mdl.putUint16(numVertices);
    mdl.putUint16(numTextures);
    mdl.putByte(lightmapped);
    mdl.putByte(rotateTexture);
    mdl.putByte(backgroundGeometry);
    mdl.putByte(shadow);
    mdl.putByte(beaming);
    mdl.putByte(static_cast<uint8_t>(mesh.render));
    mdl.putUint16(0); // unknown
    mdl.putFloat(totalArea);
    mdl.putUint32(0); // unknown
    if (_tsl) {
        mdl.putBytes(8, 0);
    }
    mdl.putUint32(offMdxData);
    mdl.putUint32(offVertices);

    // TODO: mesh types

    if (numVertices > 0) {
        for (size_t i = 0; i < mesh.mesh->vertices().size(); ++i) {
            mdx.putFloat(mesh.mesh->vertices()[i]);
        }
    }
}

void MdlWriter::writeArrayDef(uint32_t offset, uint32_t count, StreamWriter &mdl) {
    mdl.putUint32(offset);
    mdl.putUint32(count);
    mdl.putUint32(count);
}

} // namespace graphics

} // namespace reone
