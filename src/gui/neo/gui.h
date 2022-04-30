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

#include "control.h"

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

class Gui : boost::noncopyable {
public:
    class Builder : boost::noncopyable {
    public:
        Builder(
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc) :
            _graphicsOpt(graphicsOpt),
            _graphicsSvc(graphicsSvc) {
        }

        Builder &rootControl(std::shared_ptr<Control> control) {
            _rootControl = std::move(control);
            return *this;
        }

        std::unique_ptr<Gui> build() {
            return std::make_unique<Gui>(
                _rootControl,
                _graphicsOpt,
                _graphicsSvc);
        }

    private:
        graphics::GraphicsOptions &_graphicsOpt;
        graphics::GraphicsServices &_graphicsSvc;

        std::shared_ptr<Control> _rootControl;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc,
            resource::ResourceServices &resourceSvc) :
            _graphicsOpt(graphicsOpt),
            _graphicsSvc(graphicsSvc),
            _resourceSvc(resourceSvc) {
        }

        std::unique_ptr<Gui> load(const std::string &resRef);

    private:
        graphics::GraphicsOptions &_graphicsOpt;
        graphics::GraphicsServices &_graphicsSvc;
        resource::ResourceServices &_resourceSvc;

        std::unique_ptr<Control> loadRootControl(const resource::Gff &gui);
        std::unique_ptr<Control> loadControl(const resource::Gff &gui);
    };

    Gui(
        std::shared_ptr<Control> rootControl,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc) :
        _rootControl(std::move(rootControl)),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc) {
    }

    bool handle(const SDL_Event &e);
    void update(float delta);
    void render();

private:
    std::shared_ptr<Control> _rootControl;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
};

} // namespace neo

} // namespace gui

} // namespace reone
