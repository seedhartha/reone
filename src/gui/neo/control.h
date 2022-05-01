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

#include "../../graphics/types.h"

#include "../types.h"

namespace reone {

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace resource {

struct ResourceServices;

class Gff;

} // namespace resource

namespace gui {

namespace neo {

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
        glm::vec3 color {0.0f};
    };

    struct Text {
        TextAlignment alignment {TextAlignment::LeftTop};
        glm::vec3 color {0.0f};
        std::string font;
        std::string text;
        int strref {-1};
    };

    virtual void load(const resource::Gff &gui, const glm::vec4 &scale);

    void append(std::shared_ptr<Control> child) {
        _children.push_back(std::move(child));
    }

    bool handle(const SDL_Event &e);
    void update(float delta);
    void render();

    Control *findControlByTag(const std::string &tag);

    int id() const {
        return _id;
    }

    const std::string &tag() const {
        return _tag;
    }

    const glm::ivec4 &extent() const {
        return _extent;
    }

    void setExtent(glm::ivec4 extent) {
        _extent = std::move(extent);
    }

    void setBorder(std::unique_ptr<Border> border) {
        _border = std::move(border);
    }

    void setHilight(std::unique_ptr<Border> hilight) {
        _hilight = std::move(hilight);
    }

    void setText(std::unique_ptr<Text> text) {
        _text = std::move(text);
    }

    void setFlipVertical(bool flip) {
        _flipVertical = flip;
    }

    void setEnabled(bool enabled) {
        _enabled = enabled;
    }

protected:
    Control(
        int id,
        ControlType type,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _id(id),
        _type(type),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    int _id;
    ControlType _type;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::string _tag;
    glm::ivec4 _extent {0};
    std::vector<std::shared_ptr<Control>> _children;

    std::unique_ptr<Border> _border;
    std::unique_ptr<Border> _hilight;
    std::unique_ptr<Text> _text;

    bool _flipVertical {false};

    bool _enabled {true};
    bool _clickable {false};
    bool _hovered {false};

    bool isInExtent(float x, float y) const;

    void getTextPlacement(glm::ivec2 &outPosition, graphics::TextGravity &outGravity) const;
};

} // namespace neo

} // namespace gui

} // namespace reone
