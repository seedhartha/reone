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

#include "gr2file.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "glm/gtc/type_ptr.hpp"

#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"

#include "jbafile.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace pt = boost::property_tree;

namespace reone {

namespace tor {

Gr2File::Gr2File(string resRef, vector<shared_ptr<Animation>> animations, shared_ptr<Model> skeleton) :
    BinaryFile(4, "GAWB"),
    _resRef(move(resRef)),
    _animations(move(animations)),
    _skeleton(move(skeleton)) {
}

void Gr2File::doLoad() {
    // Adapted from multiple sources:
    //
    // https://github.com/SWTOR-Extractors-Modders-Dataminers/Granny2-Plug-In-Blender-2.8x/blob/v1.0.0.1/io_scene_gr2/import_gr2.py
    // https://forum.xentax.com/viewtopic.php?f=16&t=9703&start=30#p94880
    // https://forum.xentax.com/viewtopic.php?f=16&t=11317&start=15#p128702

    seek(0x10);

    uint32_t numCachedOffsets = readUint32();
    _fileType = static_cast<FileType>(readUint32());
    _numMeshes = readUint16();
    _numMaterials = readUint16();
    _numBones = readUint16();
    _numAttachments = readUint16();

    seek(0x50);

    uint32_t offsetCachedOffsets = readUint32();
    _offsetMeshHeader = readUint32();
    _offsetMaterialHeader = readUint32();
    _offsetBoneStructure = readUint32();
    _offsetAttachments = readUint32();

    loadMaterials();
    loadMeshes();
    loadSkeletonBones();
    loadAttachments();
    loadModel();
}

void Gr2File::loadMeshes() {
    for (uint16_t i = 0; i < _numMeshes; ++i) {
        seek(_offsetMeshHeader + i * 0x28);
        _meshes.push_back(readMesh());
    }
}

unique_ptr<Gr2File::Gr2Mesh> Gr2File::readMesh() {
    uint32_t offsetName = readUint32();

    auto mesh = make_unique<Gr2Mesh>();
    mesh->header.name = readCStringAt(offsetName);

    ignore(4);

    mesh->header.numPieces = readUint16();
    mesh->header.numUsedBones = readUint16();
    mesh->header.vertexMask = readUint16();
    mesh->header.vertexSize = readUint16();
    mesh->header.numVertices = readUint32();
    mesh->header.numIndices = readUint32();
    mesh->header.offsetVertices = readUint32();
    mesh->header.offsetPieces = readUint32();
    mesh->header.offsetIndices = readUint32();
    mesh->header.offsetBones = readUint32();

    for (uint16_t i = 0; i < mesh->header.numPieces; ++i) {
        seek(mesh->header.offsetPieces + i * 0x30);
        mesh->pieces.push_back(readMeshPiece());
    }

    mesh->mesh = readModelMesh(*mesh);

    for (uint16_t i = 0; i < mesh->header.numUsedBones; ++i) {
        seek(mesh->header.offsetBones + i * 0x1c);
        mesh->bones.push_back(readMeshBone());
    }

    return move(mesh);
}

unique_ptr<Gr2File::MeshPiece> Gr2File::readMeshPiece() {
    auto piece = make_unique<MeshPiece>();
    piece->startFaceIdx = readUint32();
    piece->numFaces = readUint32();
    piece->materialIndex = readUint32();
    piece->pieceIndex = readUint32();

    ignore(0x24); // bounding box

    return move(piece);
}

static float convertByteToFloat(uint8_t value) {
    return 2.0f * value / 255.0f - 1.0f;
}

static float convertHalfFloatToFloat(uint16_t value) {
    uint32_t sign = (value & 0x8000) ? 1 : 0;
    uint32_t exponent = ((value & 0x7c00) >> 10) - 16;
    uint32_t mantissa = value & 0x03ff;
    uint32_t tmp = ((sign << 31) | ((exponent + 127) << 23) | (mantissa << 13));
    float result = 2.0f * *reinterpret_cast<float *>(&tmp);
    return result;
}

static glm::vec3 computeBitangent(float sign, const glm::vec3 &normal, const glm::vec3 &tangent) {
    return sign * glm::cross(tangent, normal);
}

unique_ptr<ModelMesh> Gr2File::readModelMesh(const Gr2Mesh &mesh) {
    int unkFlags = mesh.header.vertexMask & ~0x1f2;
    if (unkFlags != 0) {
        warn(boost::format("GR2: unrecognized vertex flags: 0x%x") % unkFlags);
    }

    Mesh::VertexOffsets offsets;

    vector<float> vertices;
    seek(mesh.header.offsetVertices);
    vector<uint8_t> gr2Vertices(readArray<uint8_t>(static_cast<size_t>(mesh.header.numVertices) * mesh.header.vertexSize));
    const uint8_t *gr2VerticesPtr = &gr2Vertices[0];
    for (uint32_t i = 0; i < mesh.header.numVertices; ++i) {
        int stride = 0;
        int gr2Stride = 0;

        // Vertex coordinates
        vertices.push_back(*reinterpret_cast<const float *>(gr2VerticesPtr + gr2Stride + 0));
        vertices.push_back(*reinterpret_cast<const float *>(gr2VerticesPtr + gr2Stride + 4));
        vertices.push_back(*reinterpret_cast<const float *>(gr2VerticesPtr + gr2Stride + 8));
        stride += 3 * sizeof(float);
        gr2Stride += 3 * sizeof(float);

        // Bone weights and indices
        if (mesh.header.vertexMask & 0x100) {
            vertices.push_back(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 0) / 255.0f);
            vertices.push_back(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 1) / 255.0f);
            vertices.push_back(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 2) / 255.0f);
            vertices.push_back(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 3) / 255.0f);
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 4)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 5)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 6)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 7)));
            offsets.boneWeights = stride;
            offsets.boneIndices = stride + 4 * sizeof(float);
            stride += 8 * sizeof(float);
            gr2Stride += 8;
        }

        // Normal and tangent space
        if (mesh.header.vertexMask & 0x2) {
            glm::vec3 normal;
            normal.x = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 0));
            normal.y = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 1));
            normal.z = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 2));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            glm::vec3 tangent;
            tangent.x = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 4));
            tangent.y = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 5));
            tangent.z = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 6));
            float sign = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 7));
            vertices.push_back(tangent.x);
            vertices.push_back(tangent.y);
            vertices.push_back(tangent.z);

            glm::vec3 bitangent(computeBitangent(sign, normal, tangent));
            vertices.push_back(bitangent.x);
            vertices.push_back(bitangent.y);
            vertices.push_back(bitangent.z);

            offsets.normals = stride;
            offsets.tangents = stride + 3 * sizeof(float);
            offsets.bitangents = stride + 6 * sizeof(float);
            stride += 9 * sizeof(float);
            gr2Stride += 8;
        }

        // Color
        if (mesh.header.vertexMask & 0x10) {
            gr2Stride += 4;
        }

        // Texture 1 coordinates
        if (mesh.header.vertexMask & 0x20) {
            vertices.push_back(convertHalfFloatToFloat(*reinterpret_cast<const uint16_t *>(gr2VerticesPtr + gr2Stride + 0)));
            vertices.push_back(-1.0f * convertHalfFloatToFloat(*reinterpret_cast<const uint16_t *>(gr2VerticesPtr + gr2Stride + 2)));
            offsets.texCoords1 = stride;
            stride += 2 * sizeof(float);
            gr2Stride += 2 * sizeof(uint16_t);
        }

        // Texture 2 coordinates
        if (mesh.header.vertexMask & 0x40) {
            gr2Stride += 4;
        }

        // Texture 3 coordinates
        if (mesh.header.vertexMask & 0x80) {
            gr2Stride += 4;
        }

        gr2VerticesPtr += mesh.header.vertexSize;
        offsets.stride = stride;
    }

    vector<uint16_t> indices;
    seek(mesh.header.offsetIndices);
    for (uint16_t i = 0; i < mesh.header.numPieces; ++i) {
        vector<uint16_t> pieceIndices(readArray<uint16_t>(3 * mesh.pieces[i]->numFaces));
        indices.insert(indices.end(), pieceIndices.begin(), pieceIndices.end());
    }

    auto simpleMesh = make_unique<Mesh>(mesh.header.numVertices, move(vertices), move(indices), move(offsets));
    simpleMesh->computeAABB();

    auto modelMesh = make_unique<ModelMesh>(move(simpleMesh), true, 0, true);
    modelMesh->setDiffuseColor(glm::vec3(0.8f));
    modelMesh->setAmbientColor(glm::vec3(0.2f));

    // Fill mesh textures from materials
    if (!_materials.empty()) {
        // TODO: add support for multiple materials
        string materialResRef(_materials[0]);
        if (materialResRef == "default") {
            materialResRef = _resRef + "_v01";
        }
        shared_ptr<ByteArray> matData(Resources::instance().get(materialResRef, ResourceType::Mat));
        if (matData) {
            pt::ptree tree;
            pt::read_xml(*wrap(matData), tree);

            for (auto &material : tree.get_child("Material")) {
                if (material.first != "input") continue;

                string semantic(material.second.get("semantic", ""));
                string type(material.second.get("type", ""));

                if (type != "texture") continue;

                string value(material.second.get("value", ""));
                int lastSlashIdx = value.find_last_of('\\');
                if (lastSlashIdx != -1) {
                    value = value.substr(lastSlashIdx + 1ll);
                }
                if (semantic == "DiffuseMap") {
                    modelMesh->setDiffuseTexture(Textures::instance().get(value, TextureType::Diffuse));
                } else if (semantic == "RotationMap1") {
                    modelMesh->setBumpmapTexture(Textures::instance().get(value, TextureType::Bumpmap), true);
                }
            }
        }
    }

    return move(modelMesh);
}

unique_ptr<Gr2File::MeshBone> Gr2File::readMeshBone() {
    uint32_t offsetName = readUint32();

    auto bone = make_unique<MeshBone>();
    bone->name = readCStringAt(offsetName);
    bone->bounds = readArray<float>(6);

    return move(bone);
}

void Gr2File::loadMaterials() {
    if (_numMaterials == 0) {
        for (auto &mesh : _meshes) {
            if (mesh->header.vertexMask & 0x20) {
                for (uint16_t i = 0; i < mesh->header.numPieces; ++i) {
                    _materials.push_back(str(boost::format("%s.%03d") % mesh->header.name % i));
                }
            }
        }
        return;
    }
    seek(_offsetMaterialHeader);
    for (uint16_t i = 0; i < _numMaterials; ++i) {
        uint32_t offsetName = readUint32();
        _materials.push_back(readCStringAt(offsetName));
    }
}

void Gr2File::loadSkeletonBones() {
    for (uint16_t i = 0; i < _numBones; ++i) {
        seek(_offsetBoneStructure + i * 0x88);
        _bones.push_back(readSkeletonBone());
    }
}

unique_ptr<Gr2File::SkeletonBone> Gr2File::readSkeletonBone() {
    uint32_t offsetName = readUint32();
    uint32_t parentIndex = readUint32();

    ignore(0x40);

    vector<float> rootToBoneValues(readArray<float>(16));

    auto bone = make_unique<SkeletonBone>();
    bone->name = readCStringAt(offsetName);
    bone->parentIndex = parentIndex;
    bone->rootToBone = glm::make_mat4(&rootToBoneValues[0]);

    return move(bone);
}

void Gr2File::loadAttachments() {
    for (uint16_t i = 0; i < _numAttachments; ++i) {
        seek(_offsetAttachments + i * 0x48);
        _attachments.push_back(readAttachment());
    }
}

unique_ptr<Gr2File::Attachment> Gr2File::readAttachment() {
    uint32_t offsetName = readUint32();
    uint32_t offsetBoneName = readUint32();
    vector<float> transformValues(readArray<float>(16));

    auto attachment = make_unique<Attachment>();
    attachment->name = readCStringAt(offsetName);
    attachment->boneName = readCStringAt(offsetBoneName);
    attachment->transform = glm::make_mat4(&transformValues[0]);

    return move(attachment);
}

static glm::mat4 getModelMatrix() {
    glm::mat4 transform(1.0f);
    transform *= glm::mat4_cast(glm::angleAxis(glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)));
    transform *= glm::mat4_cast(glm::angleAxis(glm::half_pi<float>(), glm::vec3(1.0f, 0.0, 0.0f)));
    transform = glm::scale(transform, glm::vec3(10.0f));
    return move(transform);
}

void Gr2File::loadModel() {
    shared_ptr<ModelNode> rootNode;
    int nodeIndex = 0;

    if (_fileType == FileType::Skeleton) {
        rootNode = make_shared<ModelNode>(nodeIndex++);
        rootNode->setName(_resRef + "_root");
        rootNode->setAbsoluteTransform(getModelMatrix());

        // Convert bones to model nodes
        vector<shared_ptr<ModelNode>> nodes;
        for (uint16_t i = 0; i < _numBones; ++i) {
            auto node = make_shared<ModelNode>(nodeIndex);
            node->setNodeNumber(nodeIndex);
            node->setName(_bones[i]->name);
            node->setAbsoluteTransform(rootNode->absoluteTransform() * glm::inverse(_bones[i]->rootToBone));
            nodes.push_back(move(node));
            ++nodeIndex;
        }

        // Reparent model nodes
        for (uint16_t i = 0; i < _numBones; ++i) {
            if (_bones[i]->parentIndex == 0xffffffff) {
                rootNode->addChild(nodes[i]);
            } else {
                nodes[_bones[i]->parentIndex]->addChild(nodes[i]);
            }
        }

    } else {
        // If skeleton is present, use it as the base
        if (_skeleton) {
            rootNode = _skeleton->rootNode();
            nodeIndex = _skeleton->maxNodeIndex() + 1;
        } else {
            rootNode = make_shared<ModelNode>(nodeIndex++);
            rootNode->setName(_resRef + "_root");
            rootNode->setAbsoluteTransform(getModelMatrix());
        }

        // Convert meshes to model nodes
        for (uint16_t i = 0; i < _numMeshes; ++i) {
            auto node = make_shared<ModelNode>(nodeIndex);
            node->setNodeNumber(nodeIndex);
            node->setName(_meshes[i]->header.name);
            node->setMesh(_meshes[i]->mesh);

            // If skeleton is present configure the model node skin
            if (_skeleton) {
                auto skin = make_shared<ModelNode::Skin>();
                for (uint16_t j = 0; j < _meshes[i]->header.numUsedBones; ++j) {
                    auto boneNode = _skeleton->findNodeByName(_meshes[i]->bones[j]->name);
                    skin->nodeIdxByBoneIdx.insert(make_pair(j, boneNode->index()));
                }
                node->setSkin(move(skin));
            }

            node->setAbsoluteTransform(rootNode->absoluteTransform());
            rootNode->addChild(move(node));
            ++nodeIndex;
        }
    }

    rootNode->computeLocalTransforms();

    _model = make_shared<Model>(_resRef, Model::Classification::Character, move(rootNode), _animations);
}

static string getSkeletonByModel(const string &modelResRef) {
    if (boost::starts_with(modelResRef, "rancor_rancor_")) return "rancor_skeleton";

    return "";
}

static vector<string> getAnimationsBySkeleton(const string &skeletonResRef) {
    vector<string> result;
    if (skeletonResRef == "rancor_skeleton") {
        result.push_back("dl_roar");
    }
    return move(result);
}

shared_ptr<Model> Gr2ModelLoader::loadModel(GameID gameId, const string &resRef) {
    shared_ptr<Model> skeletonModel;
    vector<shared_ptr<Animation>> animations;

    string skeletonResRef(getSkeletonByModel(resRef));
    if (!skeletonResRef.empty()) {
        shared_ptr<ByteArray> skeletonData(Resources::instance().get(skeletonResRef, ResourceType::Gr2));
        if (skeletonData) {
            Gr2File skeleton(skeletonResRef, vector<shared_ptr<Animation>>());
            skeleton.load(wrap(skeletonData));
            skeletonModel = skeleton.model();
        }

        vector<string> animationResRefs(getAnimationsBySkeleton(skeletonResRef));
        for (auto &animResRef : animationResRefs) {
            shared_ptr<ByteArray> jbaData(Resources::instance().get(animResRef, ResourceType::Jba));
            if (jbaData) {
                JbaFile jba(animResRef, skeletonModel);
                jba.load(wrap(jbaData));
                shared_ptr<Animation> animation(jba.animation());
                if (animation) {
                    // DEBUG: currently we only add a single animation and rename it to "cpause1"
                    animation->setName("cpause1");
                    animations.push_back(move(animation));
                    break;
                }
            }
        }
    }

    shared_ptr<ByteArray> geometryData(Resources::instance().get(resRef, ResourceType::Gr2));
    if (geometryData) {
        Gr2File geometry(resRef, move(animations), move(skeletonModel));
        geometry.load(wrap(geometryData));
        return geometry.model();
    }

    return nullptr;
}

} // namespace tor

} // namespace reone
