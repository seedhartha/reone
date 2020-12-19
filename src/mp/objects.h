/*
 * Copyright (c) 2020 The reone project contributors
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

#include <cstdint>

namespace reone {

namespace mp {

struct BaseStatus { };

// dummy structure for testing
struct CreatureStatus : public BaseStatus {
    float x { 0.0f };
    float y { 0.0f };
    float z { 0.0f };
    double heading { 0.0 };
    uint8_t anim { 0 };
    uint8_t animframe { 0 };
    uint8_t faction { 0 };		// hostile reticle
    uint16_t equipment1 { 0 };	// set equipment model
    uint16_t equipment2 { 0 };
    uint16_t equipment3 { 0 };
    uint16_t equipment4 { 0 };
    uint16_t equipment5 { 0 };
    uint16_t equipment6 { 0 };
};

} // namespace net

} // namespace reone
