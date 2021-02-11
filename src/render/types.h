/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include <string>
#include <vector>

#include "SDL2/SDL_events.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace reone {

namespace render {

static constexpr int kMaxLightCount = 4;
static constexpr int kNumCubeFaces = 6;

enum class TextureType {
    Diffuse,
    Lightmap,
    EnvironmentMap,
    IrradianceMap,
    Bumpmap,
    GUI,
    Cursor,
    ColorBuffer,
    DepthBuffer,
    CubeMapDepthBuffer
};

enum class TextureBlending {
    None,
    Additive
};

enum class CubeMapSide {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5
};

enum class TextureProcedureType {
    Invalid,
    Cycle
};

struct GraphicsOptions {
    int width { 0 };
    int height { 0 };
    bool fullscreen { false };
};

struct TextureFeatures {
    std::string envmapTexture;
    std::string bumpyShinyTexture;
    std::string bumpmapTexture;
    float bumpMapScaling { 0.0f };
    TextureBlending blending { TextureBlending::None };
    int numChars { 0 };
    float fontHeight { 0.0f };
    std::vector<glm::vec3> upperLeftCoords;
    std::vector<glm::vec3> lowerRightCoords;
    float waterAlpha { -1.0f };
    TextureProcedureType procedureType { TextureProcedureType::Invalid };
    int numX { 0 };
    int numY { 0 };
    int fps { 0 };
};

class IEventHandler {
public:
    virtual bool handle(const SDL_Event &event) = 0;
};

} // namespace render

} // namespace reone
