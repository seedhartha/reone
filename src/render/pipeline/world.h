/*
 * Copyright (c) 2020 The reone project contributors
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

#include <memory>
#include <vector>

#include "../framebuffer.h"
#include "../types.h"

namespace reone {

namespace render {

class WorldRenderPipeline {
public:
    WorldRenderPipeline(IRenderable *scene, const GraphicsOptions &opts);

    void init();
    void render() const;

private:
    IRenderable *_scene { nullptr };
    GraphicsOptions _opts;
    Framebuffer _geometry;
    Framebuffer _verticalBlur;
    Framebuffer _horizontalBlur;
    std::vector<std::unique_ptr<Framebuffer>> _shadows;

    WorldRenderPipeline(const WorldRenderPipeline &) = delete;
    WorldRenderPipeline &operator=(const WorldRenderPipeline &) = delete;

    void drawShadows() const;
    void drawGeometry() const;
    void applyHorizontalBlur() const;
    void applyVerticalBlur() const;
    void drawResult() const;
};

} // namespace render

} // namespace reone
