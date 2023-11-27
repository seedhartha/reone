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

#include "reone/system/types.h"

namespace reone {

namespace resource {

static constexpr int kMaxResRefLength = 16;

class ResRef {
public:
    ResRef(std::string value) :
        _value(std::move(value)) {
        if (_value.size() > kMaxResRefLength) {
            _value.erase(kMaxResRefLength);
        }
        boost::to_lower(_value);
    }

    inline const std::string &value() const {
        return _value;
    }

    inline bool operator==(const ResRef &rhs) const {
        return _value == rhs._value;
    }

    inline bool operator!=(const ResRef &rhs) const {
        return _value != rhs._value;
    }

private:
    std::string _value;
};

} // namespace resource

} // namespace reone
