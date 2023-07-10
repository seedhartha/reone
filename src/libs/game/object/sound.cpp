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

#include "reone/audio/files.h"
#include "reone/audio/player.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/resource/2da.h"
#include "reone/resource/2das.h"
#include "reone/resource/di/services.h"
#include "reone/resource/gffs.h"
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

void Sound::loadFromGIT(const Gff &gffs) {
    std::string templateResRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    loadFromBlueprint(templateResRef);
    loadTransformFromGIT(gffs);
}

void Sound::loadFromBlueprint(const std::string &resRef) {
    std::shared_ptr<Gff> uts(_services.resource.gffs.get(resRef, ResourceType::Uts));
    if (!uts) {
        return;
    }
    loadUTS(*uts);
}

void Sound::loadUTS(const Gff &uts) {
    _uts = std::make_unique<schema::UTS>(schema::parseUTS(uts));

    _tag = boost::to_lower_copy(uts.getString("Tag"));
    _name = _services.resource.strings.get(uts.getInt("LocName"));
    _blueprintResRef = boost::to_lower_copy(uts.getString("TemplateResRef"));
    _active = uts.getBool("Active");
    _continuous = uts.getBool("Continuous");
    _looping = uts.getBool("Looping");
    _positional = uts.getBool("Positional");
    _randomPosition = uts.getBool("RandomPosition");
    _random = uts.getInt("Random");
    _elevation = uts.getFloat("Elevation");
    _maxDistance = uts.getFloat("MaxDistance");
    _minDistance = uts.getFloat("MinDistance");
    _randomRangeX = uts.getFloat("RandomRangeX");
    _randomRangeY = uts.getFloat("RandomRangeY");
    _interval = uts.getInt("Interval");
    _intervalVrtn = uts.getInt("IntervalVrtn");
    _pitchVariation = uts.getFloat("PitchVariation");
    _volume = uts.getInt("Volume");
    _volumeVrtn = uts.getInt("VolumeVrtn");

    loadPriorityFromUTS(uts);

    for (auto &soundGffs : uts.getList("Sounds")) {
        _sounds.push_back(boost::to_lower_copy(soundGffs->getString("Sound")));
    }

    // Unused fields:
    //
    // - Hours (always 0)
    // - Times (always 3)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Sound::loadPriorityFromUTS(const Gff &uts) {
    std::shared_ptr<TwoDa> priorityGroups(_services.resource.twoDas.get("prioritygroups"));
    int priorityIdx = uts.getInt("Priority");
    _priority = priorityGroups->getInt(priorityIdx, "priority");
}

void Sound::loadTransformFromGIT(const Gff &gffs) {
    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

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
