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

#include <memory>
#include <unordered_map>
#include <vector>

#include <boost/noncopyable.hpp>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"

#include "../resource/format/gfffile.h"
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
    virtual void render();
    virtual void render3D();

    void configureRootContol(const std::function<void(Control &)> &fn);
    void configureControl(const std::string &tag, const std::function<void(Control &)> &fn);
    void disableControl(const std::string &tag);
    void enableControl(const std::string &tag);
    void hideControl(const std::string &tag);
    void resetFocus();
    void showControl(const std::string &tag);

    virtual void onClick(const std::string &control);
    virtual void onListBoxItemClick(const std::string &control, const std::string &item);

    void setControlDisabled(const std::string &tag, bool disabled);
    void setControlFocus(const std::string &tag, bool focus);
    void setControlFocusable(const std::string &tag, bool focusable);
    void setControlText(const std::string &tag, const std::string &text);
    void setControlVisible(const std::string &tag, bool visible);
    void setControlDiscardColor(const std::string &tag, glm::vec3 color);

protected:
    enum class ScalingMode {
        Center,
        PositionRelativeToCenter,
        Stretch
    };

    resource::GameID _gameId { resource::GameID::KotOR };
    render::GraphicsOptions _gfxOpts;
    std::string _resRef;
    BackgroundType _backgroundType { BackgroundType::None };
    int _resolutionX { kDefaultResolutionX };
    int _resolutionY { kDefaultResolutionY };
    ScalingMode _scaling { ScalingMode::Center };
    float _aspect { 0.0f };
    glm::ivec2 _screenCenter { 0 };
    glm::ivec2 _rootOffset { 0 };
    glm::ivec2 _controlOffset { 0 };
    std::shared_ptr<render::Texture> _background;
    std::unique_ptr<Control> _rootControl;
    std::vector<std::unique_ptr<Control>> _controls;
    std::unordered_map<std::string, Control *> _controlByTag;
    Control *_focus { nullptr };
    bool _hasDefaultHilightColor { false };
    glm::vec3 _defaultHilightColor { 0.0f };
    std::unordered_map<std::string, ScalingMode> _scalingByControlTag;

    GUI(resource::GameID gameId, const render::GraphicsOptions &opts);

    void loadBackground(BackgroundType type);
    void loadControl(const resource::GffStruct &gffs);
    virtual void onFocusChanged(const std::string &control, bool focus);
    virtual void preloadControl(Control &control);

    Control &getControl(const std::string &tag) const;
    std::string getResRef(const std::string &base) const;

    template <class T>
    T &getControl(const std::string &tag) const {
        Control &ctrl = getControl(tag);
        return static_cast<T &>(ctrl);
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
