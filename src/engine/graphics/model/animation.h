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

namespace reone {

namespace graphics {

class ModelNode;

class Animation : boost::noncopyable {
public:
    struct Event {
        float time {0.0f};
        std::string name;
    };

    Animation(
        std::string name,
        float length,
        float transitionTime,
        std::string root,
        std::shared_ptr<ModelNode> rootNode,
        std::vector<Event> events);

    std::shared_ptr<ModelNode> getNodeByNumber(uint16_t number) const;
    std::shared_ptr<ModelNode> getNodeByName(const std::string &name) const;

    const std::string &name() const { return _name; }
    float length() const { return _length; }
    float transitionTime() const { return _transitionTime; }
    const std::string &root() const { return _root; }
    std::shared_ptr<ModelNode> rootNode() const { return _rootNode; }
    const std::vector<Event> &events() const { return _events; }

private:
    std::string _name;
    float _length {0.0f};
    float _transitionTime {0.0f}; /**< time in seconds where this animation overlaps with other animations */
    std::string _root;
    std::shared_ptr<ModelNode> _rootNode;
    std::vector<Event> _events;

    std::unordered_map<uint16_t, std::shared_ptr<ModelNode>> _nodeByNumber;

    void fillLookups();
};

} // namespace graphics

} // namespace reone
