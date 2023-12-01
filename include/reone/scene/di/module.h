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

#include "reone/graphics/di/module.h"
#include "reone/graphics/options.h"
#include "reone/resource/di/module.h"

#include "../graphs.h"

#include "services.h"

namespace reone {

namespace scene {

class SceneModule : boost::noncopyable {
public:
    SceneModule(
        graphics::GraphicsOptions &graphicsOpt,
        resource::ResourceModule &resource,
        graphics::GraphicsModule &graphics) :
        _graphicsOpt(graphicsOpt),
        _resource(resource),
        _graphics(graphics) {
    }

    ~SceneModule() { deinit(); }

    void init();
    void deinit();

    SceneGraphs &graphs() { return *_graphs; }

    SceneServices &services() { return *_services; }

private:
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsModule &_graphics;
    resource::ResourceModule &_resource;

    std::unique_ptr<SceneGraphs> _graphs;

    std::unique_ptr<SceneServices> _services;
};

} // namespace scene

} // namespace reone
