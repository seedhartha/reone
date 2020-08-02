#pragma once

#include "modelnode.h"

namespace reone {

namespace render {

class Animation {
public:
    Animation(const std::string &name, float length, float transitionTime, const std::shared_ptr<ModelNode> &rootNode);

    std::shared_ptr<ModelNode> findNode(const std::string &name) const;

    const std::string &name() const;
    float length() const;
    float transitionTime() const;
    std::shared_ptr<ModelNode> rootNode() const;

private:
    std::string _name;
    float _length { 0.0f };
    float _transitionTime { 0.0f };
    std::shared_ptr<ModelNode> _rootNode;
    std::map<std::string, std::shared_ptr<ModelNode>> _nodeByName;

    Animation(const Animation &) = delete;
    Animation &operator=(const Animation &) = delete;

    friend class resources::MdlFile;
};

} // namespace render

} // namespace reone
