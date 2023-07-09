/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/animation.h"

#include "reone/graphics/modelnode.h"

namespace reone {

namespace graphics {

Animation::Animation(
    std::string name,
    float length,
    float transitionTime,
    std::string root,
    std::shared_ptr<ModelNode> rootNode,
    std::vector<Event> events) :
    _name(std::move(name)),
    _length(length),
    _transitionTime(transitionTime),
    _root(std::move(root)),
    _rootNode(std::move(rootNode)),
    _events(std::move(events)) {

    fillLookups();
}

void Animation::fillLookups() {
    std::stack<std::shared_ptr<ModelNode>> nodes;
    nodes.push(_rootNode);

    while (!nodes.empty()) {
        std::shared_ptr<ModelNode> node(nodes.top());
        nodes.pop();

        _nodeByNumber.insert(std::make_pair(node->number(), node));
        _nodeByName.insert(std::make_pair(node->name(), node));

        for (auto &child : node->children()) {
            nodes.push(child);
        }
    }
}

std::shared_ptr<ModelNode> Animation::getNodeByNumber(uint16_t number) const {
    auto it = _nodeByNumber.find(number);
    return it != _nodeByNumber.end() ? it->second : nullptr;
}

std::shared_ptr<ModelNode> Animation::getNodeByName(const std::string &name) const {
    auto it = _nodeByName.find(name);
    return it != _nodeByName.end() ? it->second : nullptr;
}

} // namespace graphics

} // namespace reone
