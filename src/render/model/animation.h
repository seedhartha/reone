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
