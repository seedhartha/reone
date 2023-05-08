/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/graphics/types.h"

#include "types.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace resource {

struct ResourceServices;

class Gff;

} // namespace resource

namespace gui {

class IControlFactory;
class IGui;

class Control : boost::noncopyable {
public:
    enum class TextAlignment {
        LeftTop = 9,
        CenterTop = 10,
        RightCenter = 12,
        LeftCenter = 17,
        CenterCenter = 18,
        RightCenter2 = 20,
        CenterBottom = 34
    };

    struct Border {
        std::string corner;
        std::string edge;
        std::string fill;
        int dimension {0};
        glm::vec3 color {1.0f};
    };

    struct Text {
        TextAlignment alignment {TextAlignment::LeftTop};
        glm::vec3 color {1.0f};
        std::string font;
        std::string text;
        int strref {-1};
    };

    virtual void load(const resource::Gff &gui, const glm::vec4 &scale);

    void append(Control &child) {
        _children.push_back(&child);
    }

    virtual bool handle(const SDL_Event &e);
    virtual void update(float delta);
    virtual void render();

    Control *findControlByTag(const std::string &tag);
    Control *pickControlAt(int x, int y);

    bool isInFocus() const {
        return _focus;
    }

    bool isInFocusRecursive() const {
        if (_focus) {
            return true;
        }
        for (auto &child : _children) {
            if (child->_focus) {
                return true;
            }
        }
        return false;
    }

    int id() const {
        return _id;
    }

    ControlType type() const {
        return _type;
    }

    const std::string &tag() const {
        return _tag;
    }

    const glm::ivec4 &extent() const {
        return _extent;
    }

    void setTag(std::string tag) {
        _tag = std::move(tag);
    }

    void setExtent(glm::ivec4 extent) {
        _extent = std::move(extent);
    }

    void setPosition(int x, int y) {
        _extent.x = x;
        _extent.y = y;
    }

    void setAlpha(float alpha) {
        _alpha = alpha;
    }

    void setBorder(std::unique_ptr<Border> border) {
        _border = std::move(border);
    }

    void setBorderFill(std::string fill) {
        if (!_border) {
            _border = std::make_unique<Border>();
        }
        _border->fill = std::move(fill);
    }

    void setHilight(std::unique_ptr<Border> hilight) {
        _hilight = std::move(hilight);
    }

    void setText(std::unique_ptr<Text> text) {
        _text = std::move(text);
    }

    void setText(std::string text) {
        if (!_text) {
            _text = std::make_unique<Text>();
        }
        _text->text = move(text);
    }

    void setFont(std::string font) {
        if (!_text) {
            _text = std::make_unique<Text>();
        }
        _text->font = font;
    }

    void setSceneGraph(scene::SceneGraph *sceneGraph) {
        _sceneGraph = sceneGraph;
    }

    void setEnabled(bool enabled) {
        _enabled = enabled;
    }

    void setFlipVertical(bool flip) {
        _flipVertical = flip;
    }

    void setFocusable(bool focusable) {
        _focusable = focusable;
    }

    void setInFocus(bool focus) {
        _focus = focus;
    }

    virtual std::shared_ptr<Control> copy(int id) {
        throw std::logic_error("Cannot copy control of type " + std::to_string(static_cast<int>(_type)));
    }

protected:
    Control(
        int id,
        ControlType type,
        IGui &gui,
        IControlFactory &controlFactory,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _id(id),
        _type(type),
        _gui(gui),
        _controlFactory(controlFactory),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    int _id;
    ControlType _type;
    IGui &_gui;
    IControlFactory &_controlFactory;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::string _tag;
    glm::ivec4 _extent {0};
    float _alpha {1.0f};

    std::unique_ptr<Border> _border;
    std::unique_ptr<Border> _hilight;
    std::unique_ptr<Text> _text;

    std::vector<Control *> _children;

    scene::SceneGraph *_sceneGraph {nullptr};

    bool _enabled {true};
    bool _flipVertical {false};
    bool _focusable {false};
    bool _focus {false};

    bool isInExtent(float x, float y) const {
        return _extent[0] <= x && x <= _extent[0] + _extent[2] &&
               _extent[1] <= y && y <= _extent[1] + _extent[3];
    }

    void getTextPlacement(glm::ivec2 &outPosition, graphics::TextGravity &outGravity) const;
};

} // namespace gui

} // namespace reone
