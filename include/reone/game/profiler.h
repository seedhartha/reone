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

namespace game {

class IProfiler {
public:
    virtual ~IProfiler() = default;

    virtual void reserveThread(std::string name,
                               std::vector<glm::vec3> colors = {}) = 0;

    virtual void measure(const std::string &threadName,
                         int timeIndex,
                         const std::function<void()> &block) = 0;
};

} // namespace game

} // namespace reone
