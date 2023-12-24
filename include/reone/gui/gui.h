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

#include "reone/graphics/di/services.h"
#include "reone/graphics/options.h"
#include "reone/input/event.h"
#include "reone/resource/di/services.h"
#include "reone/resource/template/generated/gui.h"

#include "control.h"

namespace reone {

namespace scene {

class ISceneGraphs;

} // namespace scene

namespace gui {

constexpr int kDefaultResolutionX = 640;
constexpr int kDefaultResolutionY = 480;

class IGUIEventListener {
public:
    virtual ~IGUIEventListener() = default;

    virtual void onClick(const std::string &control) = 0;
    virtual void onSelectionChanged(const std::string &control, bool selected) = 0;
};

class IGUI {
public:
    enum class ScalingMode {
        Center,
        PositionRelativeToCenter,
        Stretch
    };

    virtual ~IGUI() = default;

    virtual void load(const resource::Gff &gui) = 0;

    virtual bool handle(const input::Event &event) = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    virtual void clearSelection() = 0;

    virtual Control &rootControl() = 0;

    virtual const glm::ivec2 &rootOffset() const = 0;
    virtual const glm::ivec2 &controlOffset() const = 0;

    virtual void setEventListener(IGUIEventListener &listener) = 0;
    virtual void setResolution(int x, int y) = 0;
    virtual void setScaling(ScalingMode scaling) = 0;
    virtual void setControlScaling(const std::string &tag, ScalingMode scaling) = 0;
    virtual void setDefaultHilightColor(glm::vec3 color) = 0;
    virtual void setBackground(std::shared_ptr<graphics::Texture> texture) = 0;

    virtual std::unique_ptr<Control> newControl(ControlType type, std::string tag) = 0;
    virtual void addControl(std::shared_ptr<Control> control) = 0;

    virtual std::shared_ptr<Control> findControl(const std::string &tag) const = 0;
};

class GUI : public IGUI, boost::noncopyable {
public:
    GUI(
        graphics::GraphicsOptions &options,
        scene::ISceneGraphs &sceneGraphs,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _options(options),
        _sceneGraphs(sceneGraphs),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {

        _aspect = options.width / static_cast<float>(options.height);
        _screenCenter.x = options.width / 2;
        _screenCenter.y = options.height / 2;
    }

    void load(const resource::Gff &gui) override;

    bool handle(const input::Event &event) override;
    void update(float dt) override;
    void render() override;

    void clearSelection() override;

    Control &rootControl() override {
        return *_rootControl;
    }

    const glm::ivec2 &rootOffset() const override {
        return _rootOffset;
    }

    const glm::ivec2 &controlOffset() const override {
        return _controlOffset;
    }

    void setEventListener(IGUIEventListener &listener) override {
        _eventListener = &listener;
    }

    void setResolution(int x, int y) override {
        _resolutionX = x;
        _resolutionY = y;
    }

    void setScaling(ScalingMode scaling) override {
        _scaling = scaling;
    }

    void setControlScaling(const std::string &tag, ScalingMode scaling) override {
        _scalingByControlTag[tag] = scaling;
    }

    void setDefaultHilightColor(glm::vec3 color) override {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = color;
    }

    void setBackground(std::shared_ptr<graphics::Texture> texture) override {
        _background = texture;
    }

    std::unique_ptr<Control> newControl(ControlType type, std::string tag) override;

    void addControl(std::shared_ptr<Control> control) override;

    std::shared_ptr<Control> findControl(const std::string &tag) const override;

private:
    graphics::GraphicsOptions &_options;

    IGUIEventListener *_eventListener {nullptr};

    int _resolutionX {kDefaultResolutionX};
    int _resolutionY {kDefaultResolutionY};
    ScalingMode _scaling {ScalingMode::Center};
    float _aspect {0.0f};
    glm::ivec2 _screenCenter {0};
    glm::ivec2 _rootOffset {0};
    glm::ivec2 _controlOffset {0};
    std::shared_ptr<graphics::Texture> _background;
    std::unordered_map<std::string, ScalingMode> _scalingByControlTag;
    bool _leftMouseDown {false};

    // Controls

    std::vector<std::shared_ptr<Control>> _controls;
    std::unordered_map<std::string, std::reference_wrapper<Control>> _tagToControl;
    std::unordered_map<std::string, std::vector<std::reference_wrapper<Control>>> _controlTagToChildren;

    std::optional<std::reference_wrapper<Control>> _rootControl;
    std::optional<std::reference_wrapper<Control>> _selection;

    // END Controls

    // Services

    scene::ISceneGraphs &_sceneGraphs;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    // END Services

    // GUI Colors

    bool _hasDefaultHilightColor {false};
    glm::vec3 _defaultHilightColor {0.0f};

    // END GUI Colors

    void loadControl(const resource::generated::GUI_CONTROLS &gui);

    void onClick(const std::string &control) {
        if (_eventListener) {
            _eventListener->onClick(control);
        }
    }

    void onSelectionChanged(const std::string &control, bool selected) {
        if (_eventListener) {
            _eventListener->onSelectionChanged(control, selected);
        }
    }

    void positionRelativeToCenter(Control &control);
    void stretchControl(Control &control);
    void updateSelection(int x, int y);

    void renderBackground(graphics::IRenderPass &pass);

    std::optional<std::reference_wrapper<Control>> findControlAt(int x, int y,
                                                                 const std::function<bool(const Control &)> &test) const;

    // User input

    virtual bool handleKeyDown(input::KeyCode key);
    virtual bool handleKeyUp(input::KeyCode key);

    // END User input
};

} // namespace gui

} // namespace reone
