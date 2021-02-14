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

#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "modelnode.h"

namespace reone {

namespace render {

class MdlFile;

class Animation : boost::noncopyable {
public:
    struct Event {
        float time { 0.0f };
        std::string name;
    };

    Animation(
        const std::string &name,
        float length,
        float transitionTime,
        std::vector<Event> &&events,
        const std::shared_ptr<ModelNode> &rootNode);

    std::shared_ptr<ModelNode> findNode(const std::string &name) const;

    const std::string &name() const { return _name; }
    float length() const { return _length; }
    float transitionTime() const { return _transitionTime; }
    std::shared_ptr<ModelNode> rootNode() const { return _rootNode; }
    const std::vector<Event> &events() const { return _events; }

    void setName(std::string name);

private:
    std::string _name;
    float _length { 0.0f };
    float _transitionTime { 0.0f };
    std::vector<Event> _events;
    std::shared_ptr<ModelNode> _rootNode;

    std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nodeByName;

    void initNodeByName();

    friend class MdlFile;
};

} // namespace render

} // namespace reone
