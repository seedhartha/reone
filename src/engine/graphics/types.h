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

namespace reone {

namespace graphics {

constexpr int kNumCubeFaces = 6;

constexpr float kDefaultClipPlaneNear = 0.1f;
constexpr float kDefaultClipPlaneFar = 10000.0f;

constexpr int kMaxBones = 24;
constexpr int kMaxLights = 8;
constexpr int kMaxParticles = 64;
constexpr int kMaxCharacters = 128;
constexpr int kMaxGrassClusters = 256;
constexpr int kMaxDanglymeshConstraints = 512;

// MDL

constexpr int kMdlDataOffset = 12;

constexpr uint32_t kMdlModelFuncPtr1KotorPC = 4273776;
constexpr uint32_t kMdlModelFuncPtr2KotorPC = 4216096;
constexpr uint32_t kMdlModelFuncPtr1KotorXbox = 4254992;
constexpr uint32_t kMdlModelFuncPtr2KotorXbox = 4255008;
constexpr uint32_t kMdlModelFuncPtr1TslPC = 4285200;
constexpr uint32_t kMdlModelFuncPtr2TslPC = 4216320;
constexpr uint32_t kMdlModelFuncPtr1TslXbox = 4285872;
constexpr uint32_t kMdlModelFuncPtr2TslXbox = 4216016;

constexpr uint32_t kMdlMeshFuncPtr1KotorPC = 4216656;
constexpr uint32_t kMdlMeshFuncPtr2KotorPC = 4216672;
constexpr uint32_t kMdlMeshFuncPtr1KotorXbox = 4267376;
constexpr uint32_t kMdlMeshFuncPtr2KotorXbox = 4264048;
constexpr uint32_t kMdlMeshFuncPtr1TslPC = 4216880;
constexpr uint32_t kMdlMeshFuncPtr2TslPC = 4216896;
constexpr uint32_t kMdlMeshFuncPtr1TslXbox = 4216576;
constexpr uint32_t kMdlMeshFuncPtr2TslXbox = 4216592;

struct MdlClassification {
    static constexpr int other = 0;
    static constexpr int effect = 1;
    static constexpr int tile = 2;
    static constexpr int character = 4;
    static constexpr int door = 8;
    static constexpr int lightsaber = 0x10;
    static constexpr int placeable = 0x20;
    static constexpr int flyer = 0x40;
};

struct MdlNodeFlags {
    static constexpr int dummy = 1;
    static constexpr int light = 2;
    static constexpr int emitter = 4;
    static constexpr int camera = 8;
    static constexpr int reference = 0x10;
    static constexpr int mesh = 0x20;
    static constexpr int skin = 0x40;
    static constexpr int anim = 0x80;
    static constexpr int dangly = 0x100;
    static constexpr int aabb = 0x200;
    static constexpr int saber = 0x800;
};

// END MDL

enum class Feature {
    PBR,
    DynamicRoomLighting
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

enum class TGADataType {
    RGBA = 2,
    Grayscale = 3,
    RGBA_RLE = 10
};

enum class PolygonMode {
    Fill,
    Line
};

enum class PixelFormat {
    Grayscale,
    RGB,
    RGBA,
    BGR,
    BGRA,
    DXT1,
    DXT5,
    Depth
};

enum class CubeMapFace {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5
};

enum class BlendMode {
    None,
    Default,
    Add,
    Lighten
};

enum class TextGravity {
    LeftCenter,
    CenterBottom,
    CenterCenter,
    CenterTop,
    RightBottom,
    RightCenter,
    RightTop
};

struct TextureUnits {
    static constexpr int diffuseMap {0};
    static constexpr int lightmap {1};
    static constexpr int environmentMap {2};
    static constexpr int bumpMap {3};
    static constexpr int bloom {4};
    static constexpr int irradianceMap {5};
    static constexpr int prefilterMap {6};
    static constexpr int brdfLookup {7};
    static constexpr int shadowMap {8};
    static constexpr int shadowMapCube {9};
};

} // namespace graphics

} // namespace reone
