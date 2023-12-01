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
#include "reone/resource/template/generated/git.h"
#include "reone/resource/template/generated/uts.h"

namespace reone {

namespace resource {

class Gff;

}

namespace game {

class Sound : public Object {
public:
    Sound(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Sound,
            std::move(sceneName),
            game,
            services) {
    }

    void loadFromGIT(const resource::generated::GIT_SoundList &git);
    void loadFromBlueprint(const std::string &resRef);

    void update(float dt) override;

    bool isActive() const { return _active; }

    glm::vec3 getPosition() const;

    int priority() const { return _priority; }
    float maxDistance() const { return _maxDistance; }
    float minDistance() const { return _minDistance; }
    bool continuous() const { return _continuous; }
    float elevation() const { return _elevation; }
    bool looping() const { return _looping; }
    bool positional() const { return _positional; }

    void setActive(bool active);

private:
    bool _active {false};
    int _priority {0};
    int _soundIdx {-1};
    float _timeout {0.0f};
    float _maxDistance {0.0f};
    float _minDistance {0.0f};
    bool _continuous {false};
    float _elevation {0.0f};
    bool _looping {false};
    bool _positional {false};
    int _interval {0};
    int _volume {0};
    bool _randomPosition {false};
    int _random {0};
    float _randomRangeX {0.0f};
    float _randomRangeY {0.0f};
    int _intervalVrtn {0};
    float _pitchVariation {0.0f};
    int _volumeVrtn {0};

    std::vector<std::string> _sounds;

    void loadTransformFromGIT(const resource::generated::GIT_SoundList &git);

    void updateTransform() override;

    // Blueprint

    void loadUTS(const resource::generated::UTS &uts);
    void loadPriorityFromUTS(const resource::generated::UTS &uts);

    // END Blueprint
};

} // namespace game

} // namespace reone
