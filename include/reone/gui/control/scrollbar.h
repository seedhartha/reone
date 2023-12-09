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

#include "../control.h"

namespace reone {

namespace gui {

class ScrollBar : public Control {
public:
    struct ScrollState {
        int count {0};      /**< total number of list items */
        int numVisible {0}; /**< number of visible list items */
        int offset {0};     /**< offset into the list of items  */
    };

    ScrollBar(
        IGUI &gui,
        scene::ISceneGraphs &sceneGraphs,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Control(
            gui,
            ControlType::ScrollBar,
            sceneGraphs,
            graphicsSvc,
            resourceSvc) {
    }

    void load(const resource::generated::GUI_BASECONTROL &gui, bool protoItem) override;
    void render(const glm::ivec2 &screenSize, const glm::ivec2 &offset, graphics::IRenderPass &pass) override;

    void setScrollState(ScrollState state);

private:
    struct Direction {
        std::shared_ptr<graphics::Texture> image;
    };

    struct Thumb {
        std::shared_ptr<graphics::Texture> image;
    };

    Direction _dir;
    Thumb _thumb;
    ScrollState _state;

    void renderThumb(const glm::ivec2 &offset, graphics::IRenderPass &pass);
    void renderArrows(const glm::ivec2 &offset, graphics::IRenderPass &pass);

    void renderUpArrow(const glm::ivec2 &offset, graphics::IRenderPass &pass);
    void renderDownArrow(const glm::ivec2 &offset, graphics::IRenderPass &pass);
};

} // namespace gui

} // namespace reone
