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

namespace reone {

namespace scene {

class ISceneGraph;

class CameraSceneNode;
class ModelSceneNode;

} // namespace scene

namespace input {

struct Event;

}

namespace game {

namespace neo {

class Creature;

using AsyncTask = std::function<void()>;
using AsyncTaskExecutor = std::function<void(AsyncTask)>;

class PlayerCameraController : boost::noncopyable {
public:
    PlayerCameraController(scene::ISceneGraph &scene) :
        _scene(scene) {
    }

    bool handle(const input::Event &event);
    void update(float dt);

    void setCameraSceneNode(scene::CameraSceneNode &sceneNode) {
        _cameraSceneNode = sceneNode;
    }

    void setCameraPosition(glm::vec3 position) {
        _cameraPosition = std::move(position);
    }

    void setCameraFacing(float facing) {
        _cameraFacing = facing;
    }

    void setCameraPitch(float pitch) {
        _cameraPitch = pitch;
    }

    void setPlayerSceneNode(scene::ModelSceneNode &sceneNode) {
        _playerSceneNode = sceneNode;
    }

    void setPlayer(Creature &player) {
        _player = player;
    }

    void setGameLogicExecutor(AsyncTaskExecutor executor) {
        _gameLogicExecutor = std::move(executor);
    }

    void refreshCamera();
    void refreshPlayer();

private:
    struct CommandTypes {
        static constexpr int None = 0;
        static constexpr int MoveCameraRight = 1 << 0;
        static constexpr int MoveCameraLeft = 1 << 1;
        static constexpr int MoveCameraFront = 1 << 2;
        static constexpr int MoveCameraBack = 1 << 3;
        static constexpr int MoveCameraUp = 1 << 4;
        static constexpr int MoveCameraDown = 1 << 5;
        static constexpr int RotateCameraCW = 1 << 6;
        static constexpr int RotateCameraCCW = 1 << 7;
        static constexpr int MovePlayerFront = 1 << 8;
        static constexpr int MovePlayerBack = 1 << 9;
        static constexpr int MovePlayerLeft = 1 << 10;
        static constexpr int MovePlayerRight = 1 << 11;
    };

    scene::ISceneGraph &_scene;

    glm::vec3 _cameraPosition {0.0f};
    float _cameraFacing {0.0f};
    float _cameraPitch {0.0f};

    glm::vec3 _playerMoveDir {0.0f};

    int _commandMask {0};

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _cameraSceneNode;
    std::optional<std::reference_wrapper<scene::ModelSceneNode>> _playerSceneNode;
    std::optional<std::reference_wrapper<Creature>> _player;
    std::optional<AsyncTaskExecutor> _gameLogicExecutor;
};

} // namespace neo

} // namespace game

} // namespace reone
