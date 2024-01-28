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

#pragma once

namespace reone {

namespace scene {

struct Attribute {
    union {
        int intValue;
        float floatValue;
    };
    std::string stringValue;
};

class AttributeBag {
public:
    void clear() {
        _attributes.clear();
    }

    void add(std::string name, int value) {
        Attribute attr;
        attr.intValue = std::move(value);
        _attributes.insert({std::move(name), std::move(attr)});
    }

    void add(std::string name, float value) {
        Attribute attr;
        attr.floatValue = std::move(value);
        _attributes.insert({std::move(name), std::move(attr)});
    }

    void add(std::string name, std::string value) {
        Attribute attr;
        attr.stringValue = std::move(value);
        _attributes.insert({std::move(name), std::move(attr)});
    }

    int getInt(const std::string &name) const {
        return findInt(name).value();
    }

    std::optional<int> findInt(const std::string &name) const {
        auto it = _attributes.find(name);
        if (it == _attributes.end()) {
            return std::nullopt;
        }
        return it->second.intValue;
    }

    float getFloat(const std::string &name) const {
        return findFloat(name).value();
    }

    std::optional<float> findFloat(const std::string &name) const {
        auto it = _attributes.find(name);
        if (it == _attributes.end()) {
            return std::nullopt;
        }
        return it->second.floatValue;
    }

    std::string getString(const std::string &name) const {
        return findString(name).value();
    }

    std::optional<std::string> findString(const std::string &name) const {
        auto it = _attributes.find(name);
        if (it == _attributes.end()) {
            return std::nullopt;
        }
        return it->second.stringValue;
    }

private:
    std::map<std::string, Attribute> _attributes;
};

} // namespace scene

} // namespace reone
