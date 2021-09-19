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

#include "mdlreader.h"

#include "../../common/collectionutil.h"
#include "../../common/logutil.h"

#include "../mesh/mesh.h"
#include "../mesh/vertexattributes.h"
#include "../texture/textures.h"

#include "animation.h"
#include "model.h"
#include "models.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

static constexpr int kMdlDataOffset = 12;
static constexpr uint32_t kFunctionPtrTslPC = 4285200;
static constexpr uint32_t kFunctionPtrTslXbox = 4285872;

static constexpr int kFlagBezier = 16;

struct EmitterFlags {
    static constexpr int p2p = 1;
    static constexpr int p2pBezier = 2;
    static constexpr int affectedByWind = 4;
    static constexpr int tinted = 8;
    static constexpr int bounce = 0x10;
    static constexpr int random = 0x20;
    static constexpr int inherit = 0x40;
    static constexpr int inheritVelocity = 0x80;
    static constexpr int inheritLocal = 0x100;
    static constexpr int splat = 0x200;
    static constexpr int inheritParticle = 0x400;
    static constexpr int depthTexture = 0x800;
    static constexpr int flag13 = 0x1000;
};

// Classification

static unordered_map<uint8_t, Model::Classification> g_classifications {
    { 0, Model::Classification::Other },
    { 1, Model::Classification::Effect },
    { 2, Model::Classification::Tile },
    { 4, Model::Classification::Character },
    { 8, Model::Classification::Door },
    { 0x10, Model::Classification::Lightsaber },
    { 0x20, Model::Classification::Placeable },
    { 0x40, Model::Classification::Flyer }
};

static Model::Classification getClassification(uint8_t ordinal) {
    return getFromLookupOrElse(g_classifications, ordinal, Model::Classification::Other);
}

// END Classification

MdlReader::MdlReader(Models &models, Textures &textures) :
    BinaryReader(4, "\000\000\000\000"),
    _models(models),
    _textures(textures) {

    initControllerFn();
}

void MdlReader::load(const shared_ptr<istream> &mdl, const shared_ptr<istream> &mdx) {
    _mdxReader = make_unique<StreamReader>(mdx);

    BinaryReader::load(mdl);
}

static bool isTSLFunctionPointer(uint32_t ptr) {
    return ptr == kFunctionPtrTslPC || ptr == kFunctionPtrTslXbox;
}

void MdlReader::doLoad() {
    // File Header
    uint32_t mdlSize = readUint32();
    uint32_t mdxSize = readUint32();

    // Geometry Header
    uint32_t funcPtr1 = readUint32();
    uint32_t funcPtr2 = readUint32();
    string name(readCString(32));
    uint32_t offRootNode = readUint32();
    uint32_t numNodes = readUint32();
    ignore(6 * 4); // unknown
    uint32_t refCount = readUint32();
    uint8_t modelType = readByte();
    ignore(3); // padding

    // Model Header
    uint8_t classification = readByte();
    uint8_t subclassification = readByte();
    ignore(1); // unknown
    uint8_t affectedByFog = readByte();
    uint32_t numChildModels = readUint32();
    ArrayDefinition animationArrayDef(readArrayDefinition());
    uint32_t superModelRef = readUint32();
    vector<float> boundingBox(readFloatArray(6));
    float radius = readFloat();
    float animationScale = readFloat();
    string superModelName(boost::to_lower_copy(readCString(32)));
    uint32_t offHeadRootNode = readUint32();
    ignore(4); // unknown
    uint32_t mdxSize2 = readUint32();
    uint32_t mdxOffset = readUint32();
    ArrayDefinition nameArrayDef(readArrayDefinition());

    _tsl = isTSLFunctionPointer(funcPtr1);

    // Read node names
    vector<uint32_t> nameOffsets(readUint32Array(kMdlDataOffset + nameArrayDef.offset, nameArrayDef.count));
    readNodeNames(nameOffsets);

    // Read nodes
    shared_ptr<ModelNode> rootNode(readNode(offRootNode, nullptr));
    prepareSkinMeshes();

    // Load supermodel
    shared_ptr<Model> superModel;
    if (!superModelName.empty() && superModelName != "null") {
        superModel = _models.get(superModelName);
    }

    // Read animations
    vector<uint32_t> animOffsets(readUint32Array(kMdlDataOffset + animationArrayDef.offset, animationArrayDef.count));
    vector<shared_ptr<Animation>> animations(readAnimations(animOffsets));

    _model = make_unique<Model>(
        name,
        getClassification(classification),
        move(rootNode),
        move(animations),
        move(superModel),
        animationScale);

    _model->setAffectedByFog(affectedByFog != 0);
}

MdlReader::ArrayDefinition MdlReader::readArrayDefinition() {
    ArrayDefinition result;
    result.offset = readUint32();
    result.count = readUint32();
    result.count2 = readUint32();
    return move(result);
}

void MdlReader::readNodeNames(const vector<uint32_t> &offsets) {
    map<string, int> nameOccurences;
    for (uint32_t offset : offsets) {
        string name(boost::to_lower_copy(readCStringAt(kMdlDataOffset + offset)));
        int numOccurances = nameOccurences[name]++;
        if (numOccurances > 0) {
            debug("Duplicate model node name: " + name);
            name = str(boost::format("%s_dup%d") % name % numOccurances);
        }
        _nodeNames.push_back(move(name));
    }
}

shared_ptr<ModelNode> MdlReader::readNode(uint32_t offset, const ModelNode *parent, bool anim) {
    seek(kMdlDataOffset + offset);

    uint16_t flags = readUint16();
    uint16_t nodeId = readUint16();
    uint16_t nameIndex = readUint16();
    ignore(2); // padding
    uint32_t offRootNode = readUint32();
    uint32_t offParentNode = readUint32();
    vector<float> positionValues(readFloatArray(3));
    vector<float> orientationValues(readFloatArray(4));
    ArrayDefinition childArrayDef(readArrayDefinition());
    ArrayDefinition controllerArrayDef(readArrayDefinition());
    ArrayDefinition controllerDataArrayDef(readArrayDefinition());

    if (flags & 0xf408) {
        throw runtime_error("Unsupported MDL node flags: " + to_string(flags));
    }
    string name(_nodeNames[nameIndex]);
    glm::vec3 restPosition(glm::make_vec3(&positionValues[0]));
    glm::quat restOrientation(orientationValues[0], orientationValues[1], orientationValues[2], orientationValues[3]);

    auto node = make_shared<ModelNode>(
        name,
        move(restPosition),
        move(restOrientation),
        parent);

    node->setFlags(flags);

    if (flags & NodeFlags::mesh) {
        node->setMesh(readMesh(flags));
    }
    if (flags & NodeFlags::light) {
        node->setLight(readLight());
    }
    if (flags & NodeFlags::emitter) {
        node->setEmitter(readEmitter());
    }
    if (flags & NodeFlags::reference) {
        node->setReference(readReference());
    }
    if (!anim) {
        _nodes.push_back(node);
        _nodeFlags.insert(make_pair(name, flags));
    }

    vector<float> controllerData(readFloatArray(kMdlDataOffset + controllerDataArrayDef.offset, controllerDataArrayDef.count));
    readControllers(controllerArrayDef.offset, controllerArrayDef.count, controllerData, anim, *node);

    vector<uint32_t> childOffsets(readUint32Array(kMdlDataOffset + childArrayDef.offset, childArrayDef.count));
    for (uint32_t offset : childOffsets) {
        node->addChild(readNode(offset, node.get(), anim));
    }

    return move(node);
}

shared_ptr<ModelNode::TriangleMesh> MdlReader::readMesh(int flags) {
    // Common Mesh Header
    uint32_t funcPtr1 = readUint32();
    uint32_t funcPtr2 = readUint32();
    ArrayDefinition faceArrayDef(readArrayDefinition());
    vector<float> boundingBox(readFloatArray(6));
    float radius = readFloat();
    vector<float> average(readFloatArray(3));
    vector<float> diffuse(readFloatArray(3));
    vector<float> ambient(readFloatArray(3));
    uint32_t transprencyHint = readUint32();
    string texture1(boost::to_lower_copy(readCString(32)));
    string texture2(boost::to_lower_copy(readCString(32)));
    string texture3(boost::to_lower_copy(readCString(12)));
    string texture4(boost::to_lower_copy(readCString(12)));
    ArrayDefinition indicesCountArrayDef(readArrayDefinition());
    ArrayDefinition indicesOffsetArrayDef(readArrayDefinition());
    ArrayDefinition invCounterArrayDef(readArrayDefinition());
    ignore(3 * 4 + 8); // unknown
    uint32_t animateUV = readUint32();
    float uvDirectionX = readFloat();
    float uvDirectionY = readFloat();
    float uvJitter = readFloat();
    float uvJitterSpeed = readFloat();
    uint32_t mdxVertexSize = readUint32();
    uint32_t mdxDataFlags = readUint32();
    int offMdxVertices = readInt32();
    int offMdxNormals = readInt32();
    int offMdxVertexColors = readInt32();
    int offMdxTexCoords1 = readInt32();
    int offMdxTexCoords2 = readInt32();
    int offMdxTexCoords3 = readInt32();
    int offMdxTexCoords4 = readInt32();
    int offMdxTanSpace = readInt32();
    ignore(3 * 4); // unknown
    uint16_t numVertices = readUint16();
    uint16_t numTextures = readUint16();
    uint8_t lightmapped = readByte();
    uint8_t rotateTexture = readByte();
    uint8_t backgroundGeometry = readByte();
    uint8_t shadow = readByte();
    uint8_t beaming = readByte();
    uint8_t render = readByte();
    ignore(2); // unknown
    float totalArea = readFloat();
    ignore(4); // unknown
    if (_tsl) ignore(8);
    uint32_t offMdxData = readUint32();
    uint32_t offVertices = readUint32();

    vector<float> vertices;
    vector<uint16_t> indices;
    unordered_map<uint32_t, vector<uint32_t>> materialFaces;
    shared_ptr<ModelNode::Skin> skin;
    shared_ptr<ModelNode::DanglyMesh> danglyMesh;
    shared_ptr<ModelNode::AABBTree> aabbTree;

    VertexAttributes attributes;
    attributes.stride = mdxVertexSize;
    attributes.offCoords = offMdxVertices;
    attributes.offNormals = offMdxNormals;
    attributes.offTexCoords1 = offMdxTexCoords1;
    attributes.offTexCoords2 = offMdxTexCoords2;
    if (offMdxTanSpace != -1) {
        attributes.offBitangents = offMdxTanSpace + 0 * sizeof(float);
        attributes.offTangents = offMdxTanSpace + 3 * sizeof(float);
        attributes.offTanSpaceNormals = offMdxTanSpace + 6 * sizeof(float);
    }

    if (flags & NodeFlags::skin) {
        // Skin Mesh Header
        ignore(3 * 4); // unknown
        uint32_t offMdxBoneWeights = readUint32();
        uint32_t offMdxBoneIndices = readUint32();
        uint32_t offBones = readUint32();
        uint32_t numBones = readUint32();
        ArrayDefinition qBoneArrayDef(readArrayDefinition());
        ArrayDefinition tBoneArrayDef(readArrayDefinition());
        ignore(3 * 4); // unknown
        vector<uint16_t> boneNodeSerial(readUint16Array(16));
        ignore(4); // padding

        vector<float> boneMap(readFloatArray(kMdlDataOffset + offBones, numBones));

        skin = make_shared<ModelNode::Skin>();
        skin->boneMap = move(boneMap);
        attributes.offBoneIndices = offMdxBoneIndices;
        attributes.offBoneWeights = offMdxBoneWeights;

    } else if (flags & NodeFlags::dangly) {
        // Dangly Mesh Header
        ArrayDefinition constraintArrayDef(readArrayDefinition());
        float displacement = readFloat();
        float tightness = readFloat();
        float period = readFloat();
        uint32_t offDanglyVertices = readUint32();

        danglyMesh = make_shared<ModelNode::DanglyMesh>();
        danglyMesh->displacement = 0.5f * displacement;  // displacement is allegedly 1/2 meters per unit
        danglyMesh->tightness = tightness;
        danglyMesh->period = period;

        danglyMesh->constraints.resize(constraintArrayDef.count);
        seek(kMdlDataOffset + constraintArrayDef.offset);
        for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
            float multiplier = readFloat();
            danglyMesh->constraints[i].multiplier = glm::clamp(multiplier / 255.0f, 0.0f, 1.0f);
        }
        seek(kMdlDataOffset + offDanglyVertices);
        for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
            vector<float> positionValues(readFloatArray(3));
            danglyMesh->constraints[i].position = glm::make_vec3(&positionValues[0]);
        }

    } else if (flags & NodeFlags::aabb) {
        // AABB Mesh Header
        uint32_t offTree = readUint32();
        aabbTree = readAABBTree(offTree);

    } else if (flags & NodeFlags::saber) {
        // Lightsaber blade is a special case. It consists of four to eight
        // planes. Some of these planes are normal meshes, but some differ in
        // that their geometry is stored in the MDL, not MDX.
        //
        // Values stored in the MDL are vertex coordinates, texture coordinates
        // and normals. However, most of the vertex coordinates seem to be
        // procedurally generated based on vertices 0-7 and 88-95.

        // Saber Mesh Header
        uint32_t offSaberVertices = readUint32();
        uint32_t offTexCoords = readUint32();
        uint32_t offNormals = readUint32();
        ignore(2 * 4); // unknown

        static int referenceIndices[] { 0, 1, 2, 3, 4, 5, 6, 7, 88, 89, 90, 91, 92, 93, 94, 95 };

        seek(static_cast<size_t>(kMdlDataOffset) + offSaberVertices);
        vector<float> saberVertices(readFloatArray(3 * numVertices));

        seek(static_cast<size_t>(kMdlDataOffset) + offTexCoords);
        vector<float> texCoords(readFloatArray(2 * numVertices));

        seek(static_cast<size_t>(kMdlDataOffset) + offNormals);
        vector<float> normals(readFloatArray(3 * numVertices));

        int numVertices = 16;
        vertices.resize(8ll * numVertices);
        float *verticesPtr = &vertices[0];

        for (int i = 0; i < numVertices; ++i) {
            int referenceIdx = referenceIndices[i];

            // Vertex coordinates
            float *vertexCoordsPtr = &saberVertices[3ll * referenceIdx];
            *(verticesPtr++) = vertexCoordsPtr[0];
            *(verticesPtr++) = vertexCoordsPtr[1];
            *(verticesPtr++) = vertexCoordsPtr[2];

            // Texture coordinates
            float *texCoordsPtr = &texCoords[2ll * referenceIdx];
            *(verticesPtr++) = texCoordsPtr[0];
            *(verticesPtr++) = texCoordsPtr[1];

            // Normals
            float *normalsPtr = &normals[3ll * referenceIdx];
            *(verticesPtr++) = normalsPtr[0];
            *(verticesPtr++) = normalsPtr[1];
            *(verticesPtr++) = normalsPtr[2];
        }

        attributes.stride = 8 * sizeof(float);
        attributes.offCoords = 0;
        attributes.offTexCoords1 = 3 * sizeof(float);
        attributes.offNormals = 5 * sizeof(float);

        indices = {
            0, 13, 12, 0, 1, 13,
            1, 14, 13, 1, 2, 14,
            2, 15, 14, 2, 3, 15,
            8, 4, 5, 8, 5, 9,
            9, 5, 6, 9, 6, 10,
            10, 6, 7, 10, 7, 11
        };
    }

    // Read vertices
    if (!(flags & NodeFlags::saber) && mdxVertexSize > 0) {
        _mdxReader->seek(offMdxData);
        vertices = _mdxReader->getFloatArray(numVertices * mdxVertexSize / sizeof(float));
    }

    if (!(flags & NodeFlags::saber) && faceArrayDef.count > 0) {
        // Faces
        seek(kMdlDataOffset + faceArrayDef.offset);
        for (uint32_t i = 0; i < faceArrayDef.count; ++i) {
            vector<float> normalValues(readFloatArray(3));
            float distance = readFloat();
            uint32_t material = readUint32();
            vector<uint16_t> adjacentFaces(readUint16Array(3));
            vector<uint16_t> faceIndices(readUint16Array(3));
            materialFaces[material].push_back(i);
        }

        // Indices
        seek(kMdlDataOffset + indicesOffsetArrayDef.offset);
        uint32_t offIndices = readUint32();
        seek(kMdlDataOffset + offIndices);
        indices = readUint16Array(3 * faceArrayDef.count);
    }

    auto mesh = make_unique<Mesh>(vertices, indices, attributes);

    ModelNode::UVAnimation uvAnimation;
    if (animateUV) {
        uvAnimation.dir = glm::vec2(uvDirectionX, uvDirectionY);
    }
    shared_ptr<Texture> diffuseMap;
    if (!texture1.empty() && texture1 != "null") {
        diffuseMap = _textures.get(texture1, TextureUsage::Diffuse);
    }
    shared_ptr<Texture> lightmap;
    if (!texture2.empty()) {
        lightmap = _textures.get(texture2, TextureUsage::Lightmap);
    }

    auto nodeMesh = make_unique<ModelNode::TriangleMesh>();
    nodeMesh->mesh = move(mesh);
    nodeMesh->materialFaces = move(materialFaces);
    nodeMesh->uvAnimation = move(uvAnimation);
    nodeMesh->diffuse = glm::make_vec3(&diffuse[0]);
    nodeMesh->ambient = glm::make_vec3(&ambient[0]);
    nodeMesh->transparency = static_cast<int>(transprencyHint);
    nodeMesh->render = static_cast<bool>(render);
    nodeMesh->shadow = static_cast<bool>(shadow);
    nodeMesh->backgroundGeometry = static_cast<bool>(backgroundGeometry);
    nodeMesh->diffuseMap = move(diffuseMap);
    nodeMesh->lightmap = move(lightmap);
    nodeMesh->skin = move(skin);
    nodeMesh->danglyMesh = move(danglyMesh);
    nodeMesh->aabbTree = move(aabbTree);
    nodeMesh->saber = flags & NodeFlags::saber;

    return move(nodeMesh);
}

shared_ptr<ModelNode::AABBTree> MdlReader::readAABBTree(uint32_t offset) {
    seek(kMdlDataOffset + offset);

    vector<float> boundingBox(readFloatArray(6));
    uint32_t offChildLeft = readUint32();
    uint32_t offChildRight = readUint32();
    int faceIndex = readInt32();
    uint32_t mostSignificantPlane = readUint32();

    auto node = make_shared<ModelNode::AABBTree>();
    node->faceIndex = faceIndex;
    node->mostSignificantPlane = static_cast<ModelNode::AABBTree::Plane>(mostSignificantPlane);
    node->aabb.expand(glm::make_vec3(&boundingBox[0]));
    node->aabb.expand(glm::make_vec3(&boundingBox[3]));

    if (faceIndex == -1) {
        node->left = readAABBTree(offChildLeft);
        node->right = readAABBTree(offChildRight);
    }

    return move(node);
}

shared_ptr<ModelNode::Light> MdlReader::readLight() {
    float flareRadius  = readFloat();
    ignore(3 * 4); // unknown
    ArrayDefinition flareSizesArrayDef(readArrayDefinition());
    ArrayDefinition flarePositionsArrayDef(readArrayDefinition());
    ArrayDefinition flareColorShiftsArrayDef(readArrayDefinition());
    ArrayDefinition flareTexturesArrayDef(readArrayDefinition());
    uint32_t priority = readUint32();
    uint32_t ambientOnly = readUint32();
    uint32_t dynamicType = readUint32();
    uint32_t affectDynamic = readUint32();
    uint32_t shadow = readUint32();
    uint32_t flare = readUint32();
    uint32_t fading = readUint32();

    auto light = make_shared<ModelNode::Light>();
    light->priority = priority;
    light->ambientOnly = static_cast<bool>(ambientOnly);
    light->dynamicType = dynamicType;
    light->affectDynamic = static_cast<bool>(affectDynamic);
    light->shadow = static_cast<bool>(shadow);
    light->flareRadius = flareRadius;
    light->fading = static_cast<bool>(fading);

    int numFlares = static_cast<int>(flareTexturesArrayDef.count);
    if (numFlares > 0) {
        vector<float> flareSizes(readFloatArray(kMdlDataOffset + flareSizesArrayDef.offset, flareSizesArrayDef.count));
        vector<float> flarePositions(readFloatArray(kMdlDataOffset + flarePositionsArrayDef.offset, flarePositionsArrayDef.count));
        vector<uint32_t> texNameOffsets(readUint32Array(kMdlDataOffset + flareTexturesArrayDef.offset, flareTexturesArrayDef.count));

        vector<glm::vec3> colorShifts;
        for (int i = 0; i < numFlares; ++i) {
            seek(kMdlDataOffset + flareColorShiftsArrayDef.offset + 12 * i);
            glm::vec3 colorShift(readFloat(), readFloat(), readFloat());
            colorShifts.push_back(move(colorShift));
        }

        vector<shared_ptr<Texture>> flareTextures;
        for (int i = 0; i < numFlares; ++i) {
            seek(kMdlDataOffset + texNameOffsets[i]);
            string textureName(boost::to_lower_copy(readCString(12)));
            shared_ptr<Texture> texture(_textures.get(textureName));
            flareTextures.push_back(move(texture));
        }

        for (int i = 0; i < numFlares; ++i) {
            ModelNode::LensFlare lensFlare;
            lensFlare.texture = flareTextures[i];
            lensFlare.colorShift = colorShifts[i];
            lensFlare.position = flarePositions[i];
            lensFlare.size = flareSizes[i];
            light->flares.push_back(move(lensFlare));
        }
    }

    return move(light);
}

static ModelNode::Emitter::UpdateMode parseEmitterUpdate(const string &str) {
    auto result = ModelNode::Emitter::UpdateMode::Invalid;
    if (str == "fountain") {
        result = ModelNode::Emitter::UpdateMode::Fountain;
    } else if (str == "single") {
        result = ModelNode::Emitter::UpdateMode::Single;
    } else if (str == "explosion") {
        result = ModelNode::Emitter::UpdateMode::Explosion;
    } else if (str == "lightning") {
        result = ModelNode::Emitter::UpdateMode::Lightning;
    }
    return result;
}

static ModelNode::Emitter::RenderMode parseEmitterRender(const string &str) {
    auto result = ModelNode::Emitter::RenderMode::Invalid;
    if (str == "normal") {
        result = ModelNode::Emitter::RenderMode::Normal;
    } else if (str == "linked") {
        result = ModelNode::Emitter::RenderMode::Linked;
    } else if (str == "billboard_to_local_z") {
        result = ModelNode::Emitter::RenderMode::BillboardToLocalZ;
    } else if (str == "billboard_to_world_z") {
        result = ModelNode::Emitter::RenderMode::BillboardToWorldZ;
    } else if (str == "aligned_to_world_z") {
        result = ModelNode::Emitter::RenderMode::AlignedToWorldZ;
    } else if (str == "aligned_to_particle_dir") {
        result = ModelNode::Emitter::RenderMode::AlignedToParticleDir;
    } else if (str == "motion_blur") {
        result = ModelNode::Emitter::RenderMode::MotionBlur;
    }
    return result;
}

static ModelNode::Emitter::BlendMode parseEmitterBlend(const string &str) {
    auto result = ModelNode::Emitter::BlendMode::Invalid;
    if (str == "normal") {
        result = ModelNode::Emitter::BlendMode::Normal;
    } else if (str == "punch") {
        result = ModelNode::Emitter::BlendMode::Punch;
    } else if (str == "lighten") {
        result = ModelNode::Emitter::BlendMode::Lighten;
    }
    return result;
}

shared_ptr<ModelNode::Emitter> MdlReader::readEmitter() {
    float deadSpace = readFloat();
    float blastRadius = readFloat();
    float blastLength = readFloat();
    uint32_t branchCount = readUint32();
    float controlPointSmoothing = readFloat();
    uint32_t xGrid = readUint32();
    uint32_t yGrid = readUint32();
    ignore(4); // unknown
    string update(boost::to_lower_copy(readCString(32)));
    string render(boost::to_lower_copy(readCString(32)));
    string blend(boost::to_lower_copy(readCString(32)));
    string texture(boost::to_lower_copy(readCString(32)));
    string chunkName(boost::to_lower_copy(readCString(16)));
    uint32_t twosided = readUint32();
    uint32_t loop = readUint32();
    uint32_t renderOrder = readUint32();
    uint32_t frameBlending = readUint32();
    string depthTexture(boost::to_lower_copy(readCString(32)));
    ignore(1); // padding
    uint32_t flags = readUint32();

    auto emitter = make_shared<ModelNode::Emitter>();
    emitter->updateMode = parseEmitterUpdate(update);
    emitter->renderMode = parseEmitterRender(render);
    emitter->blendMode = parseEmitterBlend(blend);
    emitter->texture = _textures.get(texture, TextureUsage::Diffuse);
    emitter->gridSize = glm::ivec2(glm::max(xGrid, 1u), glm::max(yGrid, 1u));
    emitter->renderOrder = renderOrder;
    emitter->loop = static_cast<bool>(loop);
    emitter->p2p = flags & EmitterFlags::p2p;
    emitter->p2pBezier = flags & EmitterFlags::p2pBezier;

    return move(emitter);
}

shared_ptr<ModelNode::Reference> MdlReader::readReference() {
    string modelResRef(boost::to_lower_copy(readCString(32)));
    uint32_t reattachable = readUint32();

    auto reference = make_shared<ModelNode::Reference>();
    reference->model = _models.get(modelResRef);
    reference->reattachable = static_cast<bool>(reattachable);

    return move(reference);
}

void MdlReader::readControllers(uint32_t keyOffset, uint32_t keyCount, const vector<float> &data, bool anim, ModelNode &node) {
    uint16_t nodeFlags;
    if (anim) {
        nodeFlags = _nodeFlags.find(node.name())->second;
    } else {
        nodeFlags = node.flags();
    }

    seek(kMdlDataOffset + keyOffset);
    for (uint32_t i = 0; i < keyCount; ++i) {
        uint32_t type = readUint32();
        ignore(2); // unknown
        uint16_t numRows = readUint16();
        uint16_t timeIndex = readUint16();
        uint16_t dataIndex = readUint16();
        uint8_t numColumns = readByte();
        ignore(3); // padding

        ControllerKey key;
        key.type = type;
        key.numRows = numRows;
        key.timeIndex = timeIndex;
        key.dataIndex = dataIndex;
        key.numColumns = numColumns;

        auto fn = getControllerFn(key.type, nodeFlags);
        if (fn) {
            fn(key, data, node);
        } else {
            debug(boost::format("Unsupported MDL controller type: %d") % static_cast<int>(key.type));
        }
    }
}

void MdlReader::prepareSkinMeshes() {
    for (auto &node : _nodes) {
        if (!node->isSkinMesh()) continue;

        shared_ptr<ModelNode::Skin> skin(node->mesh()->skin);
        for (size_t i = 0; i < skin->boneMap.size(); ++i) {
            auto boneIdx = static_cast<uint16_t>(skin->boneMap[i]);
            if (boneIdx >= skin->boneNodeName.size()) {
                skin->boneNodeName.resize(boneIdx + 1);
            }
            if (boneIdx != 0xffff) {
                shared_ptr<ModelNode> boneNode(_nodes[i]);
                skin->boneNodeName[boneIdx] = boneNode->name();
            }
        }
    }
}

vector<shared_ptr<Animation>> MdlReader::readAnimations(const vector<uint32_t> &offsets) {
    vector<shared_ptr<Animation>> anims;
    anims.reserve(offsets.size());

    for (uint32_t offset : offsets) {
        anims.push_back(readAnimation(offset));
    }

    return move(anims);
}

unique_ptr<Animation> MdlReader::readAnimation(uint32_t offset) {
    seek(kMdlDataOffset + offset);

    // Geometry Header
    uint32_t funcPtr1 = readUint32();
    uint32_t funcPtr2 = readUint32();
    string name(boost::to_lower_copy(readCString(32)));
    uint32_t offRootNode = readUint32();
    uint32_t numNodes = readUint32();
    ignore(6 * 4); // unknown
    uint32_t refCount = readUint32();
    uint8_t modelType = readByte();
    ignore(3); // padding

    // Animation Header
    float length = readFloat();
    float transitionTime = readFloat();
    string root(readCString(32));
    ArrayDefinition eventArrayDef(readArrayDefinition());
    ignore(4); // unknown

    shared_ptr<ModelNode> rootNode(readNode(offRootNode, nullptr, true));

    // Events
    vector<Animation::Event> events;
    if (eventArrayDef.count > 0) {
        seek(kMdlDataOffset + eventArrayDef.offset);
        for (uint32_t i = 0; i < eventArrayDef.count; ++i) {
            Animation::Event event;
            event.time = readFloat();
            event.name = boost::to_lower_copy(readCString(32));
            events.push_back(move(event));
        }
        sort(events.begin(), events.end(), [](auto &left, auto &right) { return left.time < right.time; });
    }

    return make_unique<Animation>(
        move(name),
        length,
        transitionTime,
        move(rootNode),
        move(events));
}

void MdlReader::initControllerFn() {
    _genericControllers = unordered_map<uint32_t, ControllerFn> {
        { 8, &readPositionController },
        { 20, &readOrientationController },
        { 36, &readScaleController }
    };
    _meshControllers = unordered_map<uint32_t, ControllerFn> {
        { 100, &readSelfIllumColorController },
        { 132, &readAlphaController }
    };
    _lightControllers = unordered_map<uint32_t, ControllerFn> {
        { 76, &readColorController },
        { 88, &readRadiusController },
        { 96, &readShadowRadiusController },
        { 100, &readVerticalDisplacementController },
        { 140, &readMultiplierController }
    };
    _emitterControllers = unordered_map<uint32_t, ControllerFn> {
        { 80, &readAlphaEndController },
        { 84, &readAlphaStartController },
        { 88, &readBirthrateController },
        { 92, &readBounceCoController },
        { 96, &readCombineTimeController },
        { 100, &readDragController },
        { 104, &readFPSController },
        { 108, &readFrameEndController },
        { 112, &readFrameStartController },
        { 116, &readGravController },
        { 120, &readLifeExpController },
        { 124, &readMassController },
        { 128, &readP2PBezier2Controller },
        { 132, &readP2PBezier3Controller },
        { 136, &readParticleRotController },
        { 140, &readRandVelController },
        { 144, &readSizeStartController },
        { 148, &readSizeEndController },
        { 152, &readSizeStartYController },
        { 156, &readSizeEndYController },
        { 160, &readSpreadController },
        { 164, &readThresholdController },
        { 168, &readVelocityController },
        { 172, &readXSizeController },
        { 176, &readYSizeController },
        { 180, &readBlurLengthController },
        { 184, &readLightingDelayController },
        { 188, &readLightingRadiusController },
        { 192, &readLightingScaleController },
        { 196, &readLightingSubDivController },
        { 200, &readLightingZigZagController },
        { 216, &readAlphaMidController },
        { 220, &readPercentStartController },
        { 224, &readPercentMidController },
        { 228, &readPercentEndController },
        { 232, &readSizeMidController },
        { 236, &readSizeMidYController },
        { 240, &readRandomBirthRateController },
        { 252, &readTargetSizeController },
        { 256, &readNumControlPtsController },
        { 260, &readControlPtRadiusController },
        { 264, &readControlPtDelayController },
        { 268, &readTangentSpreadController },
        { 272, &readTangentLengthController },
        { 284, &readColorMidController },
        { 380, &readColorEndController },
        { 392, &readColorStartController },
        { 502, &readDetonateController }
    };
}

MdlReader::ControllerFn MdlReader::getControllerFn(uint32_t type, int nodeFlags) {
    ControllerFn fn;
    if (nodeFlags & NodeFlags::mesh) {
        fn = getFromLookupOrNull(_meshControllers, type);
    } else if (nodeFlags & NodeFlags::light) {
        fn = getFromLookupOrNull(_lightControllers, type);
    } else if (nodeFlags & NodeFlags::emitter) {
        fn = getFromLookupOrNull(_emitterControllers, type);
    }
    if (!fn) {
        fn = getFromLookupOrNull(_genericControllers, type);
    }
    return move(fn);
}

static inline void ensureNumColumnsEquals(int type, int expected, int actual) {
    if (actual != expected) {
        throw runtime_error(str(boost::format("Controller %d: number of columns is %d, expected %d") % type % actual % expected));
    }
}

void MdlReader::readFloatController(const ControllerKey &key, const vector<float> &data, AnimatedProperty<float> &prop) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;
    ensureNumColumnsEquals(key.type, 1, numColumns);

    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float value = data[key.dataIndex + (bezier ? 3 : 1) * i];
        prop.addFrame(time, value);
    }
    prop.update();
}

void MdlReader::readVectorController(const ControllerKey &key, const vector<float> &data, AnimatedProperty<glm::vec3> &prop) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;
    ensureNumColumnsEquals(key.type, 3, numColumns);

    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        glm::vec3 value(glm::make_vec3(&data[key.dataIndex + (bezier ? 9 : 3) * i]));
        prop.addFrame(time, value);
    }
    prop.update();
}

void MdlReader::readPositionController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.position());
}

void MdlReader::readOrientationController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    switch (key.numColumns) {
        case 2:
            for (uint16_t i = 0; i < key.numRows; ++i) {
                int rowTimeIdx = key.timeIndex + i;
                int rowDataIdx = key.dataIndex + i;

                uint32_t temp = *reinterpret_cast<const uint32_t *>(&data[rowDataIdx]);
                float x = 1.0f - static_cast<float>(temp & 0x7ff) / 1023.0f;
                float y = 1.0f - static_cast<float>((temp >> 11) & 0x7ff) / 1023.0f;
                float z = 1.0f - static_cast<float>(temp >> 22) / 511.0f;
                float dot = x * x + y * y + z * z;
                float w;

                if (dot >= 1.0f) {
                    float len = glm::sqrt(dot);
                    x /= len;
                    y /= len;
                    z /= len;
                    w = 0.0f;
                } else {
                    w = -glm::sqrt(1.0f - dot);
                }

                float time = data[rowTimeIdx];
                glm::quat orientation(w, x, y, z);
                node.orientation().addFrame(time, move(orientation));
            }
            break;
        case 4:
            for (uint16_t i = 0; i < key.numRows; ++i) {
                int rowTimeIdx = key.timeIndex + i;
                int rowDataIdx = key.dataIndex + 4 * i;

                float time = data[rowTimeIdx];

                float x = data[rowDataIdx + 0];
                float y = data[rowDataIdx + 1];
                float z = data[rowDataIdx + 2];
                float w = data[rowDataIdx + 3];
                glm::quat orientation(w, x, y, z);

                node.orientation().addFrame(time, move(orientation));
            }
            break;
        default:
            throw runtime_error("Unexpected number of columns: " + to_string(key.numColumns));
    }

    node.orientation().update();
}

void MdlReader::readScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.scale());
}

void MdlReader::readSelfIllumColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.selfIllumColor());
}

void MdlReader::readAlphaController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alpha());
}

void MdlReader::readColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.color());
}

void MdlReader::readRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.radius());
}

void MdlReader::readShadowRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.shadowRadius());
}

void MdlReader::readVerticalDisplacementController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.verticalDisplacement());
}

void MdlReader::readMultiplierController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.multiplier());
}

void MdlReader::readAlphaEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaEnd());
}

void MdlReader::readAlphaStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaStart());
}

void MdlReader::readBirthrateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.birthrate());
}

void MdlReader::readBounceCoController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.bounceCo());
}

void MdlReader::readCombineTimeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.combineTime());
}

void MdlReader::readDragController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.drag());
}

void MdlReader::readFPSController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.fps());
}

void MdlReader::readFrameEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.frameEnd());
}

void MdlReader::readFrameStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.frameStart());
}

void MdlReader::readGravController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.grav());
}

void MdlReader::readLifeExpController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lifeExp());
}

void MdlReader::readMassController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.mass());
}

void MdlReader::readP2PBezier2Controller(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.p2pBezier2());
}

void MdlReader::readP2PBezier3Controller(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.p2pBezier3());
}

void MdlReader::readParticleRotController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.particleRot());
}

void MdlReader::readRandVelController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.randVel());
}

void MdlReader::readSizeStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeStart());
}

void MdlReader::readSizeEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeEnd());
}

void MdlReader::readSizeStartYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeStartY());
}

void MdlReader::readSizeEndYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeEndY());
}

void MdlReader::readSpreadController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.spread());
}

void MdlReader::readThresholdController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.threshold());
}

void MdlReader::readVelocityController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.velocity());
}

void MdlReader::readXSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.xSize());
}

void MdlReader::readYSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.ySize());
}

void MdlReader::readBlurLengthController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.blurLength());
}

void MdlReader::readLightingDelayController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingDelay());
}

void MdlReader::readLightingRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingRadius());
}

void MdlReader::readLightingScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingScale());
}

void MdlReader::readLightingSubDivController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingSubDiv());
}

void MdlReader::readLightingZigZagController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingZigZag());
}

void MdlReader::readAlphaMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaMid());
}

void MdlReader::readPercentStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentStart());
}

void MdlReader::readPercentMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentMid());
}

void MdlReader::readPercentEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentEnd());
}

void MdlReader::readSizeMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeMid());
}

void MdlReader::readSizeMidYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeMidY());
}

void MdlReader::readRandomBirthRateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.randomBirthRate());
}

void MdlReader::readTargetSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.targetSize());
}

void MdlReader::readNumControlPtsController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.numControlPts());
}

void MdlReader::readControlPtRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.controlPtRadius());
}

void MdlReader::readControlPtDelayController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.controlPtDelay());
}

void MdlReader::readTangentSpreadController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.tangentSpread());
}

void MdlReader::readTangentLengthController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.tangentLength());
}

void MdlReader::readColorMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorMid());
}

void MdlReader::readColorEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorEnd());
}

void MdlReader::readColorStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorStart());
}

void MdlReader::readDetonateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.detonate());
}

} // namespace graphics

} // namespace reone
