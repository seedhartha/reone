/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "animation.h"

#include "reone/common/collectionutil.h"

#include "modelnode.h"

using namespace std;

namespace reone {

namespace graphics {

Animation::Animation(
    string name,
    float length,
    float transitionTime,
    string root,
    shared_ptr<ModelNode> rootNode,
    vector<Event> events) :
    _name(move(name)),
    _length(length),
    _transitionTime(transitionTime),
    _root(move(root)),
    _rootNode(move(rootNode)),
    _events(move(events)) {

    fillLookups();
}

void Animation::fillLookups() {
    stack<shared_ptr<ModelNode>> nodes;
    nodes.push(_rootNode);

    while (!nodes.empty()) {
        shared_ptr<ModelNode> node(nodes.top());
        nodes.pop();

        _nodeByNumber.insert(make_pair(node->number(), node));
        _nodeByName.insert(make_pair(node->name(), node));

        for (auto &child : node->children()) {
            nodes.push(child);
        }
    }
}

shared_ptr<ModelNode> Animation::getNodeByNumber(uint16_t number) const {
    return getFromLookupOrNull(_nodeByNumber, number);
}

shared_ptr<ModelNode> Animation::getNodeByName(const string &name) const {
    return getFromLookupOrNull(_nodeByName, name);
}

} // namespace graphics

} // namespace reone
