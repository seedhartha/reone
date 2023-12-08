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

#include "types.h"

namespace reone {

namespace graphics {

class Texture;

struct Material : boost::noncopyable {
public:
    using TextureUnit = int;
    using TextureUnitToTexture = std::unordered_map<TextureUnit, std::reference_wrapper<Texture>>;

    std::string programId;
    TextureUnitToTexture textures;
    glm::mat3x4 uv {1.0f};
    glm::vec4 color {1.0f};
    int heightMapFrame {0};

    glm::vec3 ambient {0.2f};
    glm::vec3 diffuse {0.8f};
    glm::vec3 selfIllumColor {0.0f};

    bool affectedByShadows {false};
    bool affectedByFog {false};

    std::optional<BlendMode> blending;
    std::optional<FaceCullMode> faceCulling;
    std::optional<PolygonMode> polygonMode;
};

} // namespace graphics

} // namespace reone
