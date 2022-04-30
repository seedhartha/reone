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

    class Builder : boost::noncopyable {
    public:
        Builder(
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc) :
            _graphicsOpt(graphicsOpt),
            _graphicsSvc(graphicsSvc) {
        }

        Builder &id(int id) {
            _id = id;
            return *this;
        }

        Builder &tag(std::string tag) {
            _tag = std::move(tag);
            return *this;
        }

        Builder &extent(glm::ivec4 extent) {
            _extent = std::move(extent);
            return *this;
        }

        Builder &border(Border border) {
            _border = std::move(border);
            return *this;
        }

        Builder &text(Text text) {
            _text = std::move(text);
            return *this;
        }

        Builder &tap(std::function<void(Builder &)> fn) {
            fn(*this);
            return *this;
        }

        virtual std::unique_ptr<Control> build() = 0;

    protected:
        graphics::GraphicsOptions &_graphicsOpt;
        graphics::GraphicsServices &_graphicsSvc;

        int _id;
        std::string _tag;
        glm::ivec4 _extent;
        Border _border;
        Text _text;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc) :
            _graphicsOpt(graphicsOpt),
            _graphicsSvc(graphicsSvc) {
        }

        virtual std::unique_ptr<Control> load(const resource::Gff &gui) = 0;

    protected:
        graphics::GraphicsOptions &_graphicsOpt;
        graphics::GraphicsServices &_graphicsSvc;

        virtual void configure(Builder &builder, const resource::Gff &gui);
    };

    void addChild(std::shared_ptr<Control> child) {
        _children.push_back(std::move(child));
    }

    void render();

protected:
    Control(
        int id,
        ControlType type,
        std::string tag,
        glm::ivec4 extent,
        Border border,
        Text text,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc) :
        _id(id),
        _type(type),
        _tag(std::move(tag)),
        _extent(std::move(extent)),
        _border(std::move(border)),
        _text(std::move(text)),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc) {
    }

    int _id;
    ControlType _type;
    std::string _tag;
    glm::ivec4 _extent;
    Border _border;
    Text _text;

    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;

    std::vector<std::shared_ptr<Control>> _children;
};

} // namespace neo

} // namespace gui

} // namespace reone
