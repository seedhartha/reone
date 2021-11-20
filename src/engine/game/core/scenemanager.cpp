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

#include "scenemanager.h"

#include "../../graphics/barycentricutil.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/triangleutil.h"
#include "../../scene/graphs.h"
#include "../../scene/node/grasscluster.h"
#include "../../scene/node/model.h"

#include "object/area.h"
#include "object/spatial.h"
#include "room.h"
#include "surfaces.h"
#include "types.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

void SceneManager::refresh(Area &area) {
    auto &sceneGraph = _sceneGraphs.get(kSceneMain);
    sceneGraph.clear();

    // Area properties

    sceneGraph.setAmbientLightColor(area.ambientColor());
    sceneGraph.setFogEnabled(area.fogEnabled());
    sceneGraph.setFogNear(area.fogNear());
    sceneGraph.setFogFar(area.fogFar());
    sceneGraph.setFogColor(area.fogColor());

    // Room models

    for (auto &room : area.rooms()) {
        auto modelSceneNode = room.second->model();
        if (modelSceneNode) {
            sceneGraph.addRoot(modelSceneNode);
        }
        auto &grass = area.grass();
        if (!grass.texture) {
            continue;
        }
        auto aabbNode = modelSceneNode->model().getAABBNode();
        if (!aabbNode) {
            continue;
        }
        glm::mat4 aabbTransform(glm::translate(aabbNode->absoluteTransform(), room.second->position()));
        auto grassSceneNode = sceneGraph.newGrass(room.first, glm::vec2(grass.quadSize), grass.texture, aabbNode->mesh()->lightmap);
        for (auto &material : _surfaces.getGrassSurfaceIndices()) {
            for (auto &face : aabbNode->getFacesByMaterial(material)) {
                vector<glm::vec3> vertices(aabbNode->mesh()->mesh->getTriangleCoords(face));
                float triArea = calculateTriangleArea(vertices);
                for (int i = 0; i < area.getNumGrassClusters(triArea); ++i) {
                    glm::vec3 baryPosition(getRandomBarycentric());
                    glm::vec3 position(aabbTransform * glm::vec4(barycentricToCartesian(vertices[0], vertices[1], vertices[2], baryPosition), 1.0f));
                    glm::vec2 lightmapUV(aabbNode->mesh()->mesh->getTriangleTexCoords2(face, baryPosition));
                    auto cluster = grassSceneNode->newCluster();
                    cluster->setPosition(move(position));
                    cluster->setVariant(area.getRandomGrassVariant());
                    cluster->setLightmapUV(move(lightmapUV));
                    grassSceneNode->addChild(move(cluster));
                }
            }
        }
        sceneGraph.addRoot(move(grassSceneNode));
    }

    // Objects

    for (auto &object : area.objects()) {
        shared_ptr<SceneNode> sceneNode(object->sceneNode());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
    }
}

void SceneManager::onObjectCreated(const SpatialObject &object) {
    auto sceneNode = object.sceneNode();
    if (!sceneNode) {
        return;
    }
    auto &sceneGraph = _sceneGraphs.get(kSceneMain);
    sceneGraph.addRoot(sceneNode);
}

} // namespace game

} // namespace reone
