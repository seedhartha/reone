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

#include "animation.h"

#include <queue>

using namespace std;

namespace reone {

namespace render {

Animation::Animation(
    const string &name,
    float length,
    float transitionTime,
    vector<Event> &&events,
    const shared_ptr<ModelNode> &rootNode
) :
    _name(name),
    _length(length),
    _transitionTime(transitionTime),
    _events(move(events)),
    _rootNode(rootNode) {

    initNodeByName();
}

void Animation::initNodeByName() {
    queue<shared_ptr<ModelNode>> nodes;
    nodes.push(_rootNode);

    while (!nodes.empty()) {
        shared_ptr<ModelNode> node(nodes.front());
        nodes.pop();

        _nodeByName.insert(make_pair(node->name(), node));

        const vector<shared_ptr<ModelNode>> &children = node->children();
        for (auto &child : children) {
            nodes.push(child);
        }
    }
}

shared_ptr<ModelNode> Animation::findNode(const string &name) const {
    auto it = _nodeByName.find(name);
    return it != _nodeByName.end() ? it->second : nullptr;
}

} // namespace render

} // namespace reone
