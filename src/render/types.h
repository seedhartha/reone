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

#include "SDL2/SDL_events.h"

namespace reone {

namespace render {

constexpr int kNumCubeFaces = 6;
constexpr int kMaxBones = 128;
constexpr int kMaxLights = 16;
constexpr int kMaxParticles = 32;
constexpr int kMaxCharacters = 128;
constexpr int kMaxGrassClusters = 256;

enum class PixelFormat {
    Grayscale,
    RGB,
    RGBA,
    BGR,
    BGRA,
    DXT1,
    DXT5,
    Depth,

    RG16F,
    RGB16F
};

/**
 * This is a hint to the engine when configuring texture properties.
 */
enum class TextureUsage {
    Default,
    CubeMapDefault,
    GUI,
    Diffuse,
    Lightmap,
    EnvironmentMap,
    IrradianceMap,
    PrefilterMap,
    BRDFLookup,
    Bumpmap,
    ColorBuffer,
    DepthBuffer,
    CubeMapDepthBuffer,
    Video
};

enum class CubeMapFace {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5
};

struct GraphicsOptions {
    int width { 0 };
    int height { 0 };
    bool fullscreen { false };
    int numLights { 8 };
    int shadowResolution { 0 };
};

struct TextureUnits {
    static constexpr int diffuse { 0 };
    static constexpr int lightmap { 1 };
    static constexpr int envmap { 2 };
    static constexpr int bumpmap { 3 };
    static constexpr int bloom { 4 };
    static constexpr int irradianceMap { 5 };
    static constexpr int prefilterMap { 6 };
    static constexpr int brdfLookup { 7 };
    static constexpr int shadowMap { 8 };
    static constexpr int cubeShadowMap { 9 };
};

class IEventHandler {
public:
    virtual bool handle(const SDL_Event &event) = 0;
};

} // namespace render

} // namespace reone
