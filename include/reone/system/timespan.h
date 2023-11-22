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

struct TimeSpan {
    float startInclusive {0.0f};
    float endExclusive {0.0f};

    float duration() const {
        return endExclusive - startInclusive;
    }

    bool contains(float time) const {
        return startInclusive <= time && time < endExclusive;
    }

    std::string string() const {
        return str(boost::format("[%.04f, %.04f)") % startInclusive % endExclusive);
    }
};

} // namespace reone
