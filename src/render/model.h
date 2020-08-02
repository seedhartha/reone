#pragma once

#include "aabb.h"
#include "animation.h"

namespace reone {

namespace render {

/**
 * Tree-like data structure, representing a 3D model. Contains model nodes
 * and animations. Models are cached and reused between model instances.
 *
 * @see reone::render::ModelNode
 * @see reone::render::Animation
 * @see reone::render::ModelInstance
 */
class Model {
public:
    Model(
        const std::string &name,
        const std::shared_ptr<ModelNode> &rootNode,
        const std::vector<std::shared_ptr<Animation>> &anims,
        const std::shared_ptr<Model> &superModel = nullptr);

    void initGL();

    std::shared_ptr<Animation> findAnimation(const std::string &name, const Model **model) const;
    std::shared_ptr<ModelNode> findNodeByNumber(uint16_t number) const;
    std::shared_ptr<ModelNode> findNodeByName(const std::string &name) const;

    const std::string &name() const;
    const ModelNode &rootNode() const;
    std::shared_ptr<Model> superModel() const;
    const AABB &aabb() const;
    float radiusXY() const;

private:
    std::string _name;
    std::shared_ptr<ModelNode> _rootNode;
    std::map<std::string, std::shared_ptr<Animation>> _animations;
    std::shared_ptr<Model> _superModel;
    std::map<uint16_t, std::shared_ptr<ModelNode>> _nodeByNumber;
    std::map<std::string, std::shared_ptr<ModelNode>> _nodeByName;
    AABB _aabb;
    float _radiusXY { 0.0f };

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    void init(const std::shared_ptr<ModelNode> &node);

    friend class resources::MdlFile;
};

} // namespace render

} // namespace reone
