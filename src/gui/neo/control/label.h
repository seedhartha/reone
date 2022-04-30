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

#include "../control.h"

namespace reone {

namespace gui {

namespace neo {

class Label : public Control {
public:
    class Builder : public Control::Builder {
    public:
        Builder(
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc) :
            Control::Builder(
                graphicsOpt,
                graphicsSvc) {
        }

        std::unique_ptr<Control> build() override {
            return std::make_unique<Label>(
                _id,
                _tag,
                _extent,
                _border,
                _text,
                _graphicsOpt,
                _graphicsSvc);
        }
    };

    class Loader : public Control::Loader {
    public:
        Loader(
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc) :
            Control::Loader(
                graphicsOpt,
                graphicsSvc) {
        }

        std::unique_ptr<Control> load(const resource::Gff &gui) {
            return Builder(_graphicsOpt, _graphicsSvc)
                .tap([this, &gui](auto &builder) { configure(builder, gui); })
                .build();
        }
    };

    Label(
        int id,
        std::string tag,
        glm::ivec4 extent,
        Border border,
        Text text,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc) :
        Control(
            id,
            ControlType::Label,
            std::move(tag),
            std::move(extent),
            std::move(border),
            std::move(text),
            graphicsOpt,
            graphicsSvc) {
    }
};

} // namespace neo

} // namespace gui

} // namespace reone
