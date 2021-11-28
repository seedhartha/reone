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

class ProgressBar : public Control {
public:
    ProgressBar(
        GUI &gui,
        graphics::Context &context,
        graphics::Fonts &fonts,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Window &window,
        resource::Strings &strings) :
        Control(
            gui,
            ControlType::ProgressBar,
            context,
            fonts,
            meshes,
            shaders,
            textures,
            window,
            strings) {
    }

    void load(const resource::GffStruct &gffs) override;
    void draw(const glm::ivec2 &offset, const std::vector<std::string> &text) override;

    void setValue(int value);

private:
    struct Progress {
        std::shared_ptr<graphics::Texture> fill;
    };

    Progress _progress;
    int _value {0};
};

} // namespace gui

} // namespace reone
