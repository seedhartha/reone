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

namespace gui {

class ImageButton : public Control {
public:
    ImageButton(GUI *gui);

    void load(const resource::GffStruct &gffs) override;

    void render(
        const glm::ivec2 &offset,
        const std::string &textOverride,
        const std::string &iconText,
        const std::shared_ptr<render::Texture> &iconTexture,
        const std::shared_ptr<render::Texture> &iconFrame) const;

private:
    std::shared_ptr<render::Texture> _iconFrame;
    std::shared_ptr<render::Font> _iconFont;

    void drawIcon(
        const glm::ivec2 &offset,
        const std::string &iconText,
        const std::shared_ptr<render::Texture> &iconTexture,
        const std::shared_ptr<render::Texture> &iconFrame) const;
};

} // namespace gui

} // namespace reone
