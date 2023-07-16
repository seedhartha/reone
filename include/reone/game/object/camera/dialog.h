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

#include "../../camerastyle.h"

#include "../camera.h"

namespace reone {

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

    DialogCamera(
        uint32_t id,
        CameraStyle style,
        float aspect,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Camera(
            id,
            std::move(sceneName),
            game,
            services),
        _style(std::move(style)),
        _aspect(aspect) {
    }

    void load();

    void setSpeakerPosition(glm::vec3 position);
    void setListenerPosition(glm::vec3 position);
    void setVariant(Variant variant);

private:
    CameraStyle _style;
    float _aspect;

    glm::vec3 _speakerPosition {0.0f};
    glm::vec3 _listenerPosition {0.0f};
    Variant _variant {Variant::Both};

    void updateSceneNode();
};

} // namespace game

} // namespace reone
