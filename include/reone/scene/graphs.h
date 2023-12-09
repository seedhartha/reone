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

namespace resource {

struct ResourceServices;

}

namespace scene {

class IRenderPipelineFactory;

class ISceneGraphs {
public:
    virtual ~ISceneGraphs() = default;

    virtual void reserve(std::string name) = 0;

    virtual ISceneGraph &get(const std::string &name) = 0;

    virtual std::set<std::string> sceneNames() const = 0;
};

class SceneGraphs : public ISceneGraphs, boost::noncopyable {
public:
    SceneGraphs(
        IRenderPipelineFactory &renderPipelineFactory,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        _renderPipelineFactory(renderPipelineFactory),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc),
        _resourceSvc(resourceSvc) {
    }

    void reserve(std::string name) override;

    ISceneGraph &get(const std::string &name) override;

    std::set<std::string> sceneNames() const override {
        auto names = std::set<std::string>();
        for (auto &scene : _scenes) {
            names.insert(scene.first);
        }
        return names;
    }

private:
    IRenderPipelineFactory &_renderPipelineFactory;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;
    resource::ResourceServices &_resourceSvc;

    std::unordered_map<std::string, std::shared_ptr<ISceneGraph>> _scenes;
};

} // namespace scene

} // namespace reone
