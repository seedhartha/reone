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

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../../common/aabb.h"

#include "animation.h"

namespace reone {

namespace render {

/**
 * Tree-like data structure, representing a 3D model. Contains model nodes
 * and animations. Models are cached and reused between model scene nodes.
 *
 * @see reone::render::ModelNode
 * @see reone::render::Animation
 */
class Model {
public:
    enum class Classification {
        Other,
        Effect,
        Tile,
        Character,
        Door,
        Lightsaber,
        Placeable,
        Flyer
    };

    Model(
        const std::string &name,
        const std::shared_ptr<ModelNode> &rootNode,
        std::vector<std::unique_ptr<Animation>> &anims,
        const std::shared_ptr<Model> &superModel = nullptr);

    void initGL();

    std::vector<std::string> getAnimationNames() const;
    std::shared_ptr<Animation> getAnimation(const std::string &name) const;
    std::shared_ptr<ModelNode> findNodeByNumber(uint16_t number) const;
    std::shared_ptr<ModelNode> findNodeByName(const std::string &name) const;

    Classification classification() const { return _classification; }
    const std::string &name() const { return _name; }
    ModelNode &rootNode() const { return *_rootNode; }
    float animationScale() const { return _animationScale; }
    std::shared_ptr<Model> superModel() const { return _superModel; }
    const AABB &aabb() const { return _aabb; }
    float radiusXY() const { return _radiusXY; }

    void setClassification(Classification classification);
    void setAnimationScale(float scale);

private:
    Classification _classification { Classification::Other };
    std::string _name;
    std::shared_ptr<ModelNode> _rootNode;
    std::unordered_map<std::string, std::shared_ptr<Animation>> _animations;
    std::shared_ptr<Model> _superModel;
    std::unordered_map<uint16_t, std::shared_ptr<ModelNode>> _nodeByNumber;
    std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nodeByName;
    AABB _aabb;
    float _radiusXY { 0.0f };
    float _animationScale { 1.0f };

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    void init(const std::shared_ptr<ModelNode> &node);

    friend class MdlFile;
    friend class Gr2File;
};

} // namespace render

} // namespace reone
