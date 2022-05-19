/*
 * Copyright (c) 2020-2022 The reone project contributors
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

const std::string g_gsPointLightShadows = R"END(
const int NUM_CUBE_FACES = 6;

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

out vec4 fragPosWorldSpace;

void main() {
    for (int face = 0; face < NUM_CUBE_FACES; ++face) {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) {
            fragPosWorldSpace = gl_in[i].gl_Position;
            gl_Position = uShadowLightSpace[face] * fragPosWorldSpace;
            EmitVertex();
        }
        EndPrimitive();
    }
}
)END";

const std::string g_gsDirectionalLightShadows = R"END(
const int NUM_SHADOW_CASCADES = 4;

layout(triangles) in;
layout(triangle_strip, max_vertices=12) out;

void main() {
    for (int cascade = 0; cascade < NUM_SHADOW_CASCADES; ++cascade) {
        gl_Layer = cascade;
        for (int i = 0; i < 3; ++i) {
            gl_Position = uShadowLightSpace[cascade] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
)END";

} // namespace graphics

} // namespace reone
