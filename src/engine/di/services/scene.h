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
#include "../../scene/pipeline/world.h"
#include "../../scene/scenegraph.h"

namespace reone {

namespace di {

class GraphicsServices;

class SceneServices : boost::noncopyable {
public:
    SceneServices(graphics::GraphicsOptions options, GraphicsServices &graphics);

    void init();

    scene::SceneGraph &graph() { return *_graph; }
    scene::WorldRenderPipeline &worldRenderPipeline() { return *_worldRenderPipeline; }

private:
    graphics::GraphicsOptions _options;
    GraphicsServices &_graphics;

    std::unique_ptr<scene::SceneGraph> _graph;
    std::unique_ptr<scene::WorldRenderPipeline> _worldRenderPipeline;
};

} // namespace di

} // namespace reone
