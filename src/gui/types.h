#pragma once

#include <string>
#include <memory>
#include <vector>

#include "glm/vec3.hpp"

#include "../render/texture.h"

namespace reone {

namespace gui {

enum class ControlType {
    Invalid = -1,
    Panel = 2,
    Label = 4,
    Button = 6,
    ScrollBar = 9,
    ListBox = 11,
};

enum class BackgroundType {
    None,
    Menu
};

enum class DebugMode {
    None,
    GameObjects,
    ModelNodes
};

struct HudContext {
    std::vector<std::shared_ptr<render::Texture>> partyPortraits;
};

struct DebugObject {
    std::string tag;
    std::string text;
    glm::vec3 screenCoords { 0.0f };
};

struct DebugContext {
    std::vector<DebugObject> objects;
};

struct GuiContext {
    HudContext hud;
    DebugContext debug;
};

} // namespace gui

} // namespace reone
