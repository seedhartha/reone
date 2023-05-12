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

#include "graph.h"

namespace reone {

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace audio {

struct AudioServices;

}

namespace scene {

class ISceneGraphs {
public:
    virtual ~ISceneGraphs() = default;
};

class SceneGraphs : public ISceneGraphs, boost::noncopyable {
public:
    SceneGraphs(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc) :
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc) {
    }

    void reserve(std::string name);

    SceneGraph &get(const std::string &name);

    const std::unordered_map<std::string, std::unique_ptr<SceneGraph>> &scenes() const { return _scenes; }

protected:
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;

    std::unordered_map<std::string, std::unique_ptr<SceneGraph>> _scenes;
};

} // namespace scene

} // namespace reone
