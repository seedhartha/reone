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

#include "reone/game/object/sound.h"

#include "reone/audio/mixer.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/resource/2da.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/audioclips.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/sound.h"

using namespace reone::audio;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Sound::loadFromGIT(const resource::generated::GIT_SoundList &git) {
    std::string templateResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(templateResRef);
    loadTransformFromGIT(git);
}

void Sound::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> uts(_services.resource.gffs.get(resRef, ResType::Uts));
    if (!uts) {
        return;
    }
    auto utsParsed = resource::generated::parseUTS(*uts);
    loadUTS(utsParsed);
}

void Sound::loadUTS(const resource::generated::UTS &uts) {
    _tag = boost::to_lower_copy(uts.Tag);
    _name = _services.resource.strings.getText(uts.LocName.first);
    _blueprintResRef = boost::to_lower_copy(uts.TemplateResRef);
    _active = uts.Active;
    _continuous = uts.Continuous;
    _looping = uts.Looping;
    _positional = uts.Positional;
    _randomPosition = uts.RandomPosition;
    _random = uts.Random;
    _elevation = uts.Elevation;
    _maxDistance = uts.MaxDistance;
    _minDistance = uts.MinDistance;
    _randomRangeX = uts.RandomRangeX;
    _randomRangeY = uts.RandomRangeY;
    _interval = uts.Interval;
    _intervalVrtn = uts.IntervalVrtn;
    _pitchVariation = uts.PitchVariation;
    _volume = uts.Volume;
    _volumeVrtn = uts.VolumeVrtn;

    loadPriorityFromUTS(uts);

    for (auto &soundStruct : uts.Sounds) {
        _sounds.push_back(boost::to_lower_copy(soundStruct.Sound));
    }

    // Unused fields:
    //
    // - Hours (always 0)
    // - Times (always 3)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Sound::loadPriorityFromUTS(const resource::generated::UTS &uts) {
    std::shared_ptr<TwoDA> priorityGroups(_services.resource.twoDas.get("prioritygroups"));
    int priorityIdx = uts.Priority;
    _priority = priorityGroups->getInt(priorityIdx, "priority");
}

void Sound::loadTransformFromGIT(const resource::generated::GIT_SoundList &git) {
    _position[0] = git.XPosition;
    _position[1] = git.YPosition;
    _position[2] = git.ZPosition;

    updateTransform();

    auto &sceneGraph = _services.scene.graphs.get(_sceneName);
    auto sceneNode = sceneGraph.newSound();
    sceneNode->setEnabled(_active);
    sceneNode->setPriority(_priority);
    sceneNode->setMaxDistance(_maxDistance);
    _sceneNode = std::move(sceneNode);
}

void Sound::update(float dt) {
    Object::update(dt);
    if (!_active) {
        return;
    }
    auto &sceneNode = static_cast<SoundSceneNode &>(*_sceneNode);
    if (sceneNode.isSoundPlaying()) {
        return;
    }
    if (_timeout > 0.0f) {
        _timeout = glm::max(0.0f, _timeout - dt);
        return;
    }
    const std::vector<std::string> &sounds = _sounds;
    int soundCount = static_cast<int>(sounds.size());
    if (sounds.empty()) {
        setActive(false);
        return;
    } else if (++_soundIdx >= soundCount) {
        if (_looping) {
            _soundIdx = 0;
        } else {
            setActive(false);
            return;
        }
    }
    float gain = _volume / 127.0f;
    bool loop = soundCount == 1 && _continuous;
    sceneNode.playSound(sounds[_soundIdx], gain, _positional, loop);
    _timeout = _interval / 1000.0f;
}

void Sound::updateTransform() {
    Object::updateTransform();
    if (!_sceneNode) {
        return;
    }
    glm::mat4 transform(_transform);
    transform *= glm::translate(glm::vec3(0.0f, 0.0f, _elevation));
    _sceneNode->setLocalTransform(_transform);
}

void Sound::setActive(bool active) {
    if (_active == active) {
        return;
    }
    if (_sceneNode) {
        _sceneNode->setEnabled(active);
    }
    if (active) {
        _timeout = 0.0f;
    }
    _active = active;
}

} // namespace game

} // namespace reone
