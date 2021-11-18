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

#include "../../graphics/options.h"
#include "../../scene/graphs.h"
#include "../../scene/pipeline/control.h"
#include "../../scene/pipeline/world.h"

namespace reone {

namespace di {

class GraphicsModule;

class SceneModule : boost::noncopyable {
public:
    SceneModule(graphics::GraphicsOptions options, GraphicsModule &graphics) :
        _options(std::move(options)),
        _graphics(graphics) {
    }

    void init();

    scene::SceneGraphs &sceneGraphs() { return *_sceneGraphs; }
    scene::WorldRenderPipeline &worldRenderPipeline() { return *_worldRenderPipeline; }
    scene::ControlRenderPipeline &controlRenderPipeline() { return *_controlRenderPipeline; }

private:
    graphics::GraphicsOptions _options;
    GraphicsModule &_graphics;

    std::unique_ptr<scene::SceneGraphs> _sceneGraphs;
    std::unique_ptr<scene::WorldRenderPipeline> _worldRenderPipeline;
    std::unique_ptr<scene::ControlRenderPipeline> _controlRenderPipeline;
};

} // namespace di

} // namespace reone
