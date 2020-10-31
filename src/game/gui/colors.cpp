/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "colors.h"

using namespace reone::resource;

namespace reone {

namespace game {

static glm::vec3 g_kotorBaseColor(0.0f, 0.639216f, 0.952941f);
static glm::vec3 g_kotorHilightColor(0.980392f, 1.0f, 0.0f);
static glm::vec3 g_kotorDisabledColor(0.0f, 0.349020f, 0.549020f);
static glm::vec3 g_tslBaseColor(0.192157f, 0.768627f, 0.647059f);
static glm::vec3 g_tslHilightColor(0.768627f, 0.768627f, 0.686275f);
static glm::vec3 g_tslDisabledColor(0.513725f, 0.513725f, 0.415686f);

glm::vec3 getBaseColor(GameVersion version) {
    switch (version) {
        case GameVersion::TheSithLords:
            return g_tslBaseColor;
        default:
            return g_kotorBaseColor;
    }
}

glm::vec3 getHilightColor(GameVersion version) {
    switch (version) {
        case GameVersion::TheSithLords:
            return g_tslHilightColor;
        default:
            return g_kotorHilightColor;
    }
}

glm::vec3 getDisabledColor(GameVersion version) {
    switch (version) {
        case GameVersion::TheSithLords:
            return g_tslDisabledColor;
        default:
            return g_kotorDisabledColor;
    }
}

} // namespace game

} // namespace reone
