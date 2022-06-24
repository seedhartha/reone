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

#include "../control.h"

namespace reone {

namespace gui {

class Plotter : public Control {
public:
    struct Figure {
        glm::vec3 color {1.0f};
        std::vector<glm::vec2> points;
    };

    Plotter(
        int id,
        IGui &gui,
        IControlFactory &controlFactory,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Control(
            id,
            ControlType::Plotter,
            gui,
            controlFactory,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void render() override;

    void clearFigures() {
        _figures.clear();
    }

    void addFigure(Figure figure) {
        _figures.push_back(std::move(figure));
    }

    void setAxes(glm::vec4 axes) {
        _axes = std::move(axes);
    }

private:
    glm::vec4 _axes {0.0f};
    std::vector<Figure> _figures;
};

} // namespace gui

} // namespace reone
