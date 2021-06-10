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

#include "../audio/services.h"
#include "../graphics/fonts.h"
#include "../graphics/mesh/meshes.h"
#include "../graphics/services.h"
#include "../graphics/shader/shaders.h"
#include "../graphics/texture/textures.h"
#include "../graphics/window.h"
#include "../resource/format/gffreader.h"
#include "../resource/resources.h"
#include "../resource/services.h"
#include "../resource/strings.h"
#include "../resource/types.h"

#include "control/control.h"

namespace reone {

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

    void configureRootContol(const std::function<void(Control &)> &fn);
    void configureControl(const std::string &tag, const std::function<void(Control &)> &fn);
    void disableControl(const std::string &tag);
    void enableControl(const std::string &tag);
    void hideControl(const std::string &tag);
    void resetFocus();
    void showControl(const std::string &tag);

    void setControlDisabled(const std::string &tag, bool disabled);
    void setControlFocus(const std::string &tag, bool focus);
    void setControlFocusable(const std::string &tag, bool focusable);
    void setControlText(const std::string &tag, const std::string &text);
    void setControlVisible(const std::string &tag, bool visible);
    void setControlDiscardColor(const std::string &tag, glm::vec3 color);

    virtual void onClick(const std::string &control);
    virtual void onListBoxItemClick(const std::string &control, const std::string &item);

    // Services

    graphics::GraphicsServices &graphics() { return _graphics; }
    audio::AudioServices &audio() { return _audio; }
    resource::ResourceServices &resources() { return _resources; }

    // END Services

protected:
    enum class ScalingMode {
        Center,
        PositionRelativeToCenter,
        Stretch
    };

    graphics::GraphicsOptions _options;
    graphics::GraphicsServices &_graphics;
    audio::AudioServices &_audio;
    resource::ResourceServices &_resources;

    std::string _resRef;
    int _resolutionX { kDefaultResolutionX };
    int _resolutionY { kDefaultResolutionY };
    ScalingMode _scaling { ScalingMode::Center };
    float _aspect { 0.0f };
    glm::ivec2 _screenCenter { 0 };
    glm::ivec2 _rootOffset { 0 };
    glm::ivec2 _controlOffset { 0 };
    std::shared_ptr<graphics::Texture> _background;
    std::unique_ptr<Control> _rootControl;
    std::vector<std::shared_ptr<Control>> _controls;
    std::unordered_map<std::string, Control *> _controlByTag;
    Control *_focus { nullptr };
    bool _hasDefaultHilightColor { false };
    glm::vec3 _defaultHilightColor { 0.0f };
    std::unordered_map<std::string, ScalingMode> _scalingByControlTag;

    GUI(
        graphics::GraphicsOptions options,
        graphics::GraphicsServices &graphics,
        audio::AudioServices &audio,
        resource::ResourceServices &resources);

    void loadControl(const resource::GffStruct &gffs);
    virtual void onFocusChanged(const std::string &control, bool focus);
    virtual void preloadControl(Control &control);

    Control &getControl(const std::string &tag) const;
    std::shared_ptr<Control> getControlPtr(const std::string &tag) const;

    template <class T>
    T &getControl(const std::string &tag) const {
        Control &ctrl = getControl(tag);
        return static_cast<T &>(ctrl);
    }

    template <class T>
    std::shared_ptr<T> getControlPtr(const std::string &tag) const {
        auto ctrl = getControlPtr(tag);
        return std::static_pointer_cast<T>(ctrl);
    }

    // User input

    virtual bool handleKeyDown(SDL_Scancode key);
    virtual bool handleKeyUp(SDL_Scancode key);

    // END User input

private:
    bool _leftMouseDown { false };

    void positionRelativeToCenter(Control &control);
    void stretchControl(Control &control);
    void updateFocus(int x, int y);

    void drawBackground();

    Control *getControlAt(int x, int y, const std::function<bool(const Control &)> &test) const;
};

} // namespace gui

} // namespace reone
