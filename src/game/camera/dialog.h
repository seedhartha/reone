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

#include "../types.h"

#include "../camera.h"
#include "camerastyle.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace game {

class DialogCamera : public Camera {
public:
    enum class Variant {
        Both,
        SpeakerClose,
        SpeakerFar,
        ListenerClose,
        ListenerFar
    };

    DialogCamera(float aspect, const CameraStyle &style, scene::SceneGraph &sceneGraph);

    void setSpeakerPosition(glm::vec3 position);
    void setListenerPosition(glm::vec3 position);
    void setVariant(Variant variant);
    void setFindObstacle(std::function<bool(const glm::vec3 &, const glm::vec3 &, glm::vec3 &)> fn);

private:
    scene::SceneGraph &_sceneGraph;

    glm::vec3 _speakerPosition {0.0f};
    glm::vec3 _listenerPosition {0.0f};
    Variant _variant {Variant::Both};

    void updateSceneNode();
};

} // namespace game

} // namespace reone
