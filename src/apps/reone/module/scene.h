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

#include "reone/graphics/options.h"
#include "reone/scene/graphs.h"
#include "reone/scene/services.h"

namespace reone {

class AudioModule;
class GraphicsModule;

class SceneModule : boost::noncopyable {
public:
    SceneModule(
        graphics::GraphicsOptions &graphicsOpt,
        AudioModule &audio,
        GraphicsModule &graphics) :
        _graphicsOpt(graphicsOpt),
        _audio(audio),
        _graphics(graphics) {
    }

    ~SceneModule() { deinit(); }

    void init();
    void deinit();

    scene::SceneGraphs &sceneGraphs() { return *_sceneGraphs; }

    scene::SceneServices &services() { return *_services; }

private:
    graphics::GraphicsOptions &_graphicsOpt;
    AudioModule &_audio;
    GraphicsModule &_graphics;

    std::unique_ptr<scene::SceneGraphs> _sceneGraphs;

    std::unique_ptr<scene::SceneServices> _services;
};

} // namespace reone
