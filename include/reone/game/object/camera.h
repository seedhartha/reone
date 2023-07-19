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

#include "../object.h"
#include "../schema/git.h"

namespace reone {

namespace game {

class Camera : public Object {
public:
    Camera(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Camera,
            std::move(sceneName),
            game,
            services) {
    }

    virtual bool handle(const SDL_Event &event) {
        return false;
    }

    void update(float dt) override {}

    virtual void stopMovement() {
    }

    inline std::shared_ptr<scene::CameraSceneNode> cameraSceneNode() const {
        return std::static_pointer_cast<scene::CameraSceneNode>(_sceneNode);
    }

    int cameraId() const { return _cameraId; }
    float fieldOfView() const { return _fieldOfView; }

    float facing() const { return _facing; }
    bool isMouseLookMode() const { return _mouseLookMode; }

protected:
    int _cameraId {0};
    float _fieldOfView {0.0f};

    float _facing {0.0f};
    bool _mouseLookMode {false};
};

} // namespace game

} // namespace reone
