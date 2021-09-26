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

#include "../graphics/options.h"

#include "control/control.h"

namespace reone {

namespace resource {

class GffStruct;
class Resources;
class Strings;

} // namespace resource

namespace graphics {

class Context;
class Features;
class Fonts;
class Materials;
class Meshes;
class PBRIBL;
class Shaders;
class Texture;
class Window;

} // namespace graphics

namespace gui {

constexpr int kDefaultResolutionX = 640;
constexpr int kDefaultResolutionY = 480;

class GUI : boost::noncopyable {
public:
    virtual void load();

    virtual bool handle(const SDL_Event &event);
    virtual void update(float dt);
    virtual void draw();
    virtual void draw3D();

    void resetFocus();

    std::unique_ptr<Control> newControl(ControlType type, std::string tag);

protected:
    enum class ScalingMode {
        Center,
        PositionRelativeToCenter,
        Stretch
    };

    graphics::GraphicsOptions _options;

    std::string _resRef;
    int _resolutionX {kDefaultResolutionX};
    int _resolutionY {kDefaultResolutionY};
    ScalingMode _scaling {ScalingMode::Center};
    float _aspect {0.0f};
    glm::ivec2 _screenCenter {0};
    glm::ivec2 _rootOffset {0};
    glm::ivec2 _controlOffset {0};
    std::shared_ptr<graphics::Texture> _background;
    std::unique_ptr<Control> _rootControl;
    std::vector<std::shared_ptr<Control>> _controls;
    std::unordered_map<std::string, Control *> _controlByTag;
    Control *_focus {nullptr};
    bool _hasDefaultHilightColor {false};
    glm::vec3 _defaultHilightColor {0.0f};
    std::unordered_map<std::string, ScalingMode> _scalingByControlTag;

    // Services

    graphics::Context &_context;
    graphics::Features &_features;
    graphics::Fonts &_fonts;
    graphics::Materials &_materials;
    graphics::Meshes &_meshes;
    graphics::PBRIBL &_pbrIbl;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::Window &_window;

    resource::Resources &_resources;
    resource::Strings &_strings;

    // END Services

    GUI(
        graphics::GraphicsOptions options,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Fonts &fonts,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Window &window,
        resource::Resources &resources,
        resource::Strings &strings);

    void loadControl(const resource::GffStruct &gffs);
    virtual void preloadControl(Control &control);

    std::shared_ptr<Control> getControl(const std::string &tag) const;

    template <class T>
    std::shared_ptr<T> getControl(const std::string &tag) const {
        auto ctrl = getControl(tag);
        return std::static_pointer_cast<T>(ctrl);
    }

    // User input

    virtual bool handleKeyDown(SDL_Scancode key);
    virtual bool handleKeyUp(SDL_Scancode key);

    // END User input

    virtual void onClick(const std::string &control) {}
    virtual void onFocusChanged(const std::string &control, bool focus) {}

private:
    bool _leftMouseDown {false};

    void positionRelativeToCenter(Control &control);
    void stretchControl(Control &control);
    void updateFocus(int x, int y);

    void drawBackground();

    Control *getControlAt(int x, int y, const std::function<bool(const Control &)> &test) const;
};

} // namespace gui

} // namespace reone
