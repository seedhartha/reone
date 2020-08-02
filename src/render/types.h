#pragma once

#include <string>
#include <vector>

#include "SDL2/SDL_events.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace reone {

namespace render {

enum class TextureType {
    Diffuse,
    Lightmap,
    EnvironmentMap,
    BumpMap,
    Font
};

enum class TextureBlending {
    None,
    Additive
};

enum AnimationFlags {
    kAnimationLoop = 1
};

enum class CameraType {
    FirstPerson,
    ThirdPerson
};

struct GraphicsOptions {
    int width { 0 };
    int height { 0 };
    bool fullscreen { false };
};

struct TextureFeatures {
    std::string envMapTexture;
    std::string bumpyShinyTexture;
    std::string bumpMapTexture;
    TextureBlending blending { TextureBlending::None };
    int numChars { 0 };
    float fontHeight { 0.0f };
    std::vector<glm::vec3> upperLeftCoords;
    std::vector<glm::vec3> lowerRightCoords;
};

struct CameraStyle {
    float distance { 0.0f };
    float pitch { 0.0f };
    float height { 0.0f };
    float viewAngle { 0.0f };
};

class IRenderRoot {
public:
    virtual void initGL() = 0;
    virtual void render() const = 0;
};

class IEventHandler {
public:
    virtual bool handle(const SDL_Event &event) = 0;
};

} // namespace render

} // namespace reone
