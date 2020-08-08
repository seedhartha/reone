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

#include "glm/vec3.hpp"

namespace reone {

namespace game {

glm::vec3 getKotorBaseColor() {
    static glm::vec3 color(0.0f, 0.639216f, 0.952941f);
    return color;
}

glm::vec3 getKotorHilightColor() {
    static glm::vec3 color(0.980392f, 1.0f, 0.0f);
    return color;
}

glm::vec3 getTslBaseColor() {
    static glm::vec3 color(0.192157f, 0.768627f, 0.647059f);
    return color;
}

glm::vec3 getTslHilightColor() {
    static glm::vec3 color(0.768627f, 0.768627f, 0.686275f);
    return color;
}

} // namespace game

} // namespace reone
