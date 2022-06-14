/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../../scene/graph.h"
#include "../../scene/node/model.h"

namespace reone {

namespace scene {

class MockSceneGraph : public SceneGraph {
public:
    MockSceneGraph(
        std::string name,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc) :
        SceneGraph(
            name,
            graphicsOpt,
            graphicsSvc,
            audioSvc) {
    }

    ModelSceneNode *pickModelAt(int x, int y, IUser *except = nullptr) const override {
        return _pickModelAtReturnValue;
    }

    void whenPickModelAtThenReturn(ModelSceneNode *value) {
        _pickModelAtReturnValue = value;
    }

private:
    ModelSceneNode *_pickModelAtReturnValue {nullptr};
};

class MockSceneGraphs : public SceneGraphs {
public:
    MockSceneGraphs(
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc) :
        SceneGraphs(
            graphicsOpt,
            graphicsSvc,
            audioSvc) {
    }

    void reserve(std::string name) override {
        if (_scenes.count(name) > 0) {
            return;
        }
        auto scene = std::make_unique<MockSceneGraph>(
            name,
            _graphicsOpt,
            _graphicsSvc,
            _audioSvc);

        _scenes.insert(make_pair(name, move(scene)));
    }
};

} // namespace scene

} // namespace reone
