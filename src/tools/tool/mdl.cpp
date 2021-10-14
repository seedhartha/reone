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

#include "mdl.h"

#include "../../engine/common/exception/validation.h"
#include "../../engine/graphics/mesh/mesh.h"
#include "../../engine/graphics/mesh/vertexattributes.h"
#include "../../engine/graphics/model/format/mdlwriter.h"
#include "../../engine/graphics/model/model.h"
#include "../../engine/graphics/model/modelnode.h"
#include "../../engine/graphics/types.h"

using namespace std;

using namespace reone::game;
using namespace reone::graphics;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

static constexpr float kDefaultAnimationScale = 1.0f;

static const glm::vec3 g_defaultDiffuse {0.8f};
static const glm::vec3 g_defaultAmbient {0.2f};

class AsciiMdlReader {
public:
    AsciiMdlReader(fs::path path) :
        _path(move(path)) {
    }

    void load() {
        vector<shared_ptr<ModelNode>> nodes;
        map<string, int> nodeIdxByName;

        string modelName;
        string superModelName;
        int classification = 0;
        float animationScale = kDefaultAnimationScale;
        int nodeFlags = 0;
        string nodeName;
        string nodeParent;
        glm::vec3 nodePosition(0.0f);
        glm::quat nodeOrientation(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 meshDiffuse(g_defaultDiffuse);
        glm::vec3 meshAmbient(g_defaultAmbient);
        bool meshRender = false;
        int meshNumVerts = 0;
        int meshNumTVerts = 0;
        int meshNumFaces = 0;
        vector<glm::vec3> meshVerts;
        vector<glm::vec3> meshTVerts;
        vector<Face> meshFaces;

        fs::ifstream ascii(_path);
        string line;
        while (getline(ascii, line)) {
            // remove all leading spaces
            boost::trim_left(line);

            // ignore empty lines and comments
            if (line.empty() || boost::starts_with(line, "#")) {
                continue;
            }

            // parse and execute instruction
            vector<string> tokens;
            boost::split(tokens, line, boost::is_space(), boost::token_compress_on);
            if (meshNumVerts > 0) {
                glm::vec3 vert;
                vert.x = atof(tokens[0].c_str());
                vert.y = atof(tokens[1].c_str());
                vert.z = atof(tokens[2].c_str());
                meshVerts.push_back(move(vert));
                --meshNumVerts;
                continue;
            }
            if (meshNumTVerts > 0) {
                glm::vec3 tvert;
                tvert.x = atof(tokens[0].c_str());
                tvert.y = atof(tokens[1].c_str());
                tvert.z = atof(tokens[2].c_str());
                meshTVerts.push_back(move(tvert));
                --meshNumTVerts;
                continue;
            }
            if (meshNumFaces > 0) {
                Face face;
                for (int i = 0; i < 3; ++i) {
                    face.indices[i] = atoi(tokens[i].c_str());
                }
                face.smoothGroup = atoi(tokens[3].c_str());
                for (int i = 0; i < 3; ++i) {
                    face.texIndices[i] = atoi(tokens[4 + i].c_str());
                }
                face.material = atoi(tokens[7].c_str());
                meshFaces.push_back(move(face));
                --meshNumFaces;
                continue;
            }
            if (tokens.empty()) {
                throw ValidationException("Line contains no instructions: " + line);
            }
            string instruction(boost::to_lower_copy(tokens[0]));
            if (instruction == "newmodel") {
                modelName = tokens[1];
            } else if (instruction == "setsupermodel") {
                superModelName = tokens[2];
                if (!superModelName.empty() && boost::to_lower_copy(superModelName) != "null") {
                    throw ValidationException("Super models are not implemented yet");
                }
            } else if (instruction == "classification") {
                classification = parseClassification(tokens[1]);
            } else if (instruction == "setanimationscale") {
                animationScale = atof(tokens[1].c_str());
            } else if (instruction == "node") {
                if (!nodeName.empty()) {
                    throw ValidationException("Cannot start a node because another node is already started: " + nodeName);
                }
                nodeFlags = parseNodeFlags(tokens[1]);
                nodeName = tokens[2];
                nodeParent.clear();
                nodePosition = glm::vec3(0.0f);
                nodeOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                meshDiffuse = g_defaultDiffuse;
                meshAmbient = g_defaultAmbient;
                meshRender = false;
                meshNumVerts = 0;
                meshNumTVerts = 0;
                meshNumFaces = 0;
                meshVerts.clear();
                meshTVerts.clear();
                meshFaces.clear();
            } else if (instruction == "endnode") {
                if (nodeName.empty()) {
                    throw ValidationException("Cannot end a node when it is not started");
                }
                shared_ptr<ModelNode> parent;
                if (!nodeParent.empty()) {
                    string parentName(boost::to_lower_copy(nodeParent));
                    if (parentName != "null") {
                        if (nodeIdxByName.count(parentName) == 0) {
                            throw ValidationException("Parent node not found: " + parentName);
                        }
                        parent = nodes[nodeIdxByName[parentName]];
                    }
                }
                auto modelNode = make_shared<ModelNode>(
                    nodeName,
                    nodePosition,
                    nodeOrientation,
                    parent.get());
                if (parent) {
                    parent->addChild(modelNode);
                }
                modelNode->setFlags(nodeFlags);
                if (nodeFlags & MdlNodeFlags::mesh) {
                    int numVerts = static_cast<int>(meshVerts.size());
                    // Compute normals
                    vector<glm::vec3> normals;
                    normals.resize(numVerts, glm::vec3(0.0f));
                    for (auto &face : meshFaces) {
                        int i1 = face.indices[0];
                        int i2 = face.indices[1];
                        int i3 = face.indices[2];
                        glm::vec3 normal(glm::normalize(glm::cross(
                            meshVerts[i2] - meshVerts[i1],
                            meshVerts[i3] - meshVerts[i1])));
                        normals[i1] += normal;
                        normals[i2] += normal;
                        normals[i3] += normal;
                    }
                    for (auto &normal : normals) {
                        normal = glm::normalize(normal);
                    }
                    // END Compute normals
                    vector<float> vertices;
                    for (int i = 0; i < numVerts; ++i) {
                        vertices.push_back(meshVerts[i].x);
                        vertices.push_back(meshVerts[i].y);
                        vertices.push_back(meshVerts[i].z);
                        vertices.push_back(normals[i].x);
                        vertices.push_back(normals[i].y);
                        vertices.push_back(normals[i].z);
                        if (!meshTVerts.empty()) {
                            vertices.push_back(meshTVerts[i].s);
                            vertices.push_back(meshTVerts[i].t);
                        }
                    }
                    vector<uint16_t> indices;
                    for (auto &face : meshFaces) {
                        indices.push_back(face.indices[0]);
                        indices.push_back(face.indices[1]);
                        indices.push_back(face.indices[2]);
                    }
                    VertexAttributes attributes;
                    if (!meshTVerts.empty()) {
                        attributes.stride = (3 + 3 + 2) * sizeof(float);
                        attributes.offCoords = 0 * sizeof(float);
                        attributes.offNormals = 3 * sizeof(float);
                        attributes.offTexCoords1 = 6 * sizeof(float);
                    } else {
                        attributes.stride = (3 + 3) * sizeof(float);
                        attributes.offCoords = 0 * sizeof(float);
                        attributes.offNormals = 3 * sizeof(float);
                    }
                    auto mesh = make_unique<Mesh>(move(vertices), move(indices), move(attributes));
                    auto triMesh = make_unique<ModelNode::TriangleMesh>();
                    triMesh->mesh = move(mesh);
                    triMesh->diffuse = meshDiffuse;
                    triMesh->ambient = meshAmbient;
                    triMesh->render = meshRender;
                    modelNode->setMesh(move(triMesh));
                }
                nodeIdxByName[boost::to_lower_copy(nodeName)] = static_cast<int>(nodes.size());
                nodes.push_back(move(modelNode));
                nodeName.clear();
            } else if (instruction == "parent") {
                nodeParent = tokens[1];
            } else if (instruction == "diffuse") {
                meshDiffuse.r = atof(tokens[1].c_str());
                meshDiffuse.g = atof(tokens[2].c_str());
                meshDiffuse.b = atof(tokens[3].c_str());
            } else if (instruction == "ambient") {
                meshAmbient.r = atof(tokens[1].c_str());
                meshAmbient.g = atof(tokens[2].c_str());
                meshAmbient.b = atof(tokens[3].c_str());
            } else if (instruction == "render") {
                meshRender = atoi(tokens[1].c_str()) != 0;
            } else if (instruction == "verts") {
                meshNumVerts = atoi(tokens[1].c_str());
            } else if (instruction == "tverts") {
                meshNumTVerts = atoi(tokens[1].c_str());
            } else if (instruction == "faces") {
                meshNumFaces = atoi(tokens[1].c_str());
            }
        }

        _model = make_shared<Model>(
            move(modelName),
            classification,
            nodes[0],
            vector<shared_ptr<Animation>>(),
            nullptr,
            animationScale);
    }

    shared_ptr<Model> model() { return _model; }

private:
    struct Face {
        int indices[3] {0};
        int smoothGroup {0};
        int texIndices[3] {0};
        int material {0};
    };

    fs::path _path;

    shared_ptr<Model> _model;

    int parseClassification(const string &str) {
        if (str == "other") {
            return MdlClassification::other;
        } else if (str == "effect") {
            return MdlClassification::effect;
        } else if (str == "tile") {
            return MdlClassification::tile;
        } else if (str == "character") {
            return MdlClassification::character;
        } else if (str == "door") {
            return MdlClassification::door;
        } else if (str == "lightsaber") {
            return MdlClassification::lightsaber;
        } else if (str == "placeable") {
            return MdlClassification::placeable;
        } else if (str == "flyer") {
            return MdlClassification::flyer;
        } else {
            throw ValidationException("Unsupported model classification: " + str);
        }
    }

    int parseNodeFlags(const string &str) {
        if (str == "dummy") {
            return MdlNodeFlags::dummy;
        } else if (str == "trimesh") {
            return MdlNodeFlags::dummy | MdlNodeFlags::mesh;
        } else {
            throw ValidationException("Unsupported node type: " + str);
        }
    }
};

void MdlTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToMDL) {
        toMDL(target, destPath);
    }
}

void MdlTool::toMDL(const fs::path &path, const fs::path &destPath) {
    AsciiMdlReader reader(path);
    reader.load();

    fs::path mdlPath(destPath);
    mdlPath.append(path.filename().string());
    mdlPath.replace_extension(); // drop .ascii

    fs::path mdxPath(mdlPath);
    mdxPath.replace_extension("mdx");

    MdlWriter writer(*reader.model(), _gameId == GameID::TSL);
    writer.save(mdlPath, mdxPath);
}

bool MdlTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           target.extension() == ".ascii" &&
           operation == Operation::ToMDL;
}

} // namespace tools

} // namespace reone
