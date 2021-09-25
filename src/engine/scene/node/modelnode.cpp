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

#include "modelnode.h"

#include "../../common/guardutil.h"
#include "../../graphics/model/modelnode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

ModelNodeSceneNode::ModelNodeSceneNode(shared_ptr<ModelNode> modelNode, SceneNodeType type, SceneGraph *sceneGraph) :
    SceneNode(modelNode->name(), type, sceneGraph),
    _modelNode(modelNode) {

    ensurePresent(modelNode, "modelNode");
}

} // namespace scene

} // namespace reone
