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

constexpr float kDefaultClipPlaneNear = 0.1f;
constexpr float kDefaultClipPlaneFar = 10000.0f;
constexpr float kDefaultObjectDrawDistance = 64.0f;

constexpr int kNumCubeFaces = 6;
constexpr int kNumShadowCascades = 8;
constexpr int kNumShadowLightSpace = 8;

constexpr int kMaxBones = 24;
constexpr int kMaxLights = 16;
constexpr int kMaxParticles = 64;
constexpr int kMaxTextChars = 128;
constexpr int kMaxGrassClusters = 256;

/**
 * This is a hint to the engine when configuring texture properties.
 */
enum class TextureUsage {
    Default,
    DefaultCubeMap,
    GUI,
    Diffuse,
    Lightmap,
    EnvironmentMap,
    BumpMap,
    ColorBuffer,
    DepthBuffer,
    DepthBufferCubeMap,
    Video,
    Lookup
};

enum class TextureQuality {
    High,
    Medium,
    Low
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
    R16F,
    DXT1,
    DXT5,
    Depth,
    DepthStencil
};

enum class CubeMapFace {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5
};

enum class CullFaceMode {
    None,
    Front,
    Back
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

enum class ShaderType {
    Vertex,
    Geometry,
    Fragment
};

enum class CameraType {
    Orthographic,
    Perspective
};

struct TextureUnits {
    static constexpr int diffuseMap = 0;
    static constexpr int lightmap = 1;
    static constexpr int bumpMap = 2;
    static constexpr int bloom = 3;
    static constexpr int gBufDepth = 4;
    static constexpr int gBufNormals = 5;
    static constexpr int gBufRoughness = 6;
    static constexpr int danglyConstraints = 7;
    static constexpr int environmentMap = 8;
    static constexpr int cubeShadowMap = 9;
    static constexpr int shadowMap = 10;
};

struct UniformBlockBindingPoints {
    static constexpr int general = 0;
    static constexpr int text = 1;
    static constexpr int lighting = 2;
    static constexpr int skeletal = 3;
    static constexpr int particles = 4;
    static constexpr int grass = 5;
};

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

} // namespace graphics

} // namespace reone
