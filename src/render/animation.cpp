#include "animation.h"

#include <queue>

namespace reone {

namespace render {

Animation::Animation(const std::string &name, float length, float transitionTime, const std::shared_ptr<ModelNode> &rootNode) :
    _name(name), _length(length), _transitionTime(transitionTime), _rootNode(rootNode) {

    std::queue<std::shared_ptr<ModelNode>> nodes;
    nodes.push(_rootNode);

    while (!nodes.empty()) {
        std::shared_ptr<ModelNode> node(nodes.front());
        nodes.pop();

        _nodeByName.insert(std::make_pair(node->name(), node));

        const std::vector<std::shared_ptr<ModelNode>> &children = node->children();
        for (auto &child : children) {
            nodes.push(child);
        }
    }
}

std::shared_ptr<ModelNode> Animation::findNode(const std::string &name) const {
    auto it = _nodeByName.find(name);
    return it != _nodeByName.end() ? it->second : nullptr;
}

const std::string &Animation::name() const {
    return _name;
}

float Animation::length() const {
    return _length;
}

float Animation::transitionTime() const {
    return _transitionTime;
}

std::shared_ptr<ModelNode> Animation::rootNode() const {
    return _rootNode;
}

} // namespace render

} // namespace reone
