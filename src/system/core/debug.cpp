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

#include "debug.h"

namespace reone {

static uint32_t g_debugLevel = 0;
static DebugMode g_debugMode = DebugMode::None;

uint32_t getDebugLevel() {
    return g_debugLevel;
}

DebugMode getDebugMode() {
    return g_debugMode;
}

void setDebugLevel(uint32_t level) {
    g_debugLevel = level;
}

void setDebugMode(DebugMode mode) {
    g_debugMode = mode;
}

} // namespace reone
