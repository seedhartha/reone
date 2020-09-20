/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "lightnode.h"

#include "../model/modelnode.h"

#include "scenegraph.h"

namespace reone {

namespace render {

LightSceneNode::LightSceneNode(const ModelNode *modelNode) : _modelNode(modelNode) {
    assert(_modelNode);
}

void LightSceneNode::fill(SceneGraph *graph) {
    graph->addLight(this);
}

const ModelNode &LightSceneNode::modelNode() const {
    return *_modelNode;
}

} // namespace render

} // namespace reone
