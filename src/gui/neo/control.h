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

#include "../types.h"

namespace reone {

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace resource {

class Gff;

}

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
        RightCenter2 = 20
    };

    struct Border {
        std::string corner;
        std::string edge;
        std::string fill;
    };

    struct Text {
        TextAlignment alignment {TextAlignment::LeftTop};
        std::string font;
        std::string text;
        int strref {-1};
    };

    void load(const resource::Gff &gui);

    void append(std::shared_ptr<Control> child) {
        _children.push_back(std::move(child));
    }

    void render();

    const glm::ivec4 &extent() const {
        return _extent;
    }

    void setExtent(glm::ivec4 extent) {
        _extent = std::move(extent);
    }

    void setBorder(Border border) {
        _border = std::move(border);
    }

    void setText(Text text) {
        _text = std::move(text);
    }

protected:
    Control(
        int id,
        ControlType type,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc) :
        _id(id),
        _type(type),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc) {
    }

    int _id;
    ControlType _type;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;

    std::string _tag;
    glm::ivec4 _extent {0};
    Border _border;
    Text _text;

    std::vector<std::shared_ptr<Control>> _children;
};

} // namespace neo

} // namespace gui

} // namespace reone
