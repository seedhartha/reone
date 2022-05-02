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

#include "../camerastyle.h"
#include "../object.h"

namespace reone {

namespace resource {

class Gff;

}

namespace game {

class Camera : public Object {
public:
    enum class Mode {
        ThirdPerson,
        Flycam
    };

    Camera(
        uint32_t id,
        ObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Object(
            id,
            ObjectType::Camera,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {

        _pitch = glm::half_pi<float>();
    }

    void loadFromGit(const resource::Gff &git);
    void loadFromStyle(const CameraStyle &style);

    bool handle(const SDL_Event &e);
    void update(float delta);

    void setMode(Camera::Mode mode) {
        _mode = mode;
    }

    void setThirdPersonHook(scene::SceneNode *hook) {
        _hook = hook;
    }

private:
    Mode _mode {Mode::Flycam};
    CameraStyle _style;
    float _aspect {1.0f};
    scene::SceneNode *_hook {nullptr};

    // Controls
    float _forward {0.0f};
    float _left {0.0f};
    float _backward {0.0f};
    float _right {0.0f};

    bool handleThirdPerson(const SDL_Event &e);
    bool handleFlycam(const SDL_Event &e);

    void updateThirdPerson(float delta);
    void updateFlycam(float delta);

    void flushProjection();
};

} // namespace game

} // namespace reone
