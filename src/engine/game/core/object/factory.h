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

#include "../../../graphics/types.h"
#include "../../../resource/types.h"

#include "../types.h"

#include "area.h"
#include "creature.h"
#include "door.h"
#include "encounter.h"
#include "module.h"
#include "placeable.h"
#include "placeablecamera.h"
#include "sound.h"
#include "trigger.h"
#include "waypoint.h"

namespace reone {

namespace resource {

class Resources;
class Strings;
class TwoDas;

} // namespace resource

namespace graphics {

class Context;
class Meshes;
class Models;
class Shaders;
class Textures;
class Walkmeshes;
class Window;

} // namespace graphics

namespace audio {

class AudioFiles;
class AudioPlayer;

} // namespace audio

namespace scene {

class SceneGraph;

}

namespace game {

class ActionFactory;
class Classes;
class Combat;
class FootstepSounds;
class Game;
class ObjectFactory;
class Party;
class Portraits;
class Reputes;
class ScriptRunner;
class SoundSets;
class Surfaces;

class ObjectFactory {
public:
    ObjectFactory(
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        FootstepSounds &footstepSounds,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        resource::Resources &resources,
        resource::Strings &strings,
        resource::TwoDas &twoDas,
        scene::SceneGraph &sceneGraph) :
        _actionFactory(actionFactory),
        _classes(classes),
        _combat(combat),
        _footstepSounds(footstepSounds),
        _party(party),
        _portraits(portraits),
        _reputes(reputes),
        _scriptRunner(scriptRunner),
        _soundSets(soundSets),
        _surfaces(surfaces),
        _audioFiles(audioFiles),
        _audioPlayer(audioPlayer),
        _context(context),
        _meshes(meshes),
        _models(models),
        _shaders(shaders),
        _textures(textures),
        _walkmeshes(walkmeshes),
        _window(window),
        _resources(resources),
        _strings(strings),
        _twoDas(twoDas),
        _sceneGraph(sceneGraph) {
    }

    std::shared_ptr<Module> newModule();
    std::shared_ptr<Area> newArea();
    std::shared_ptr<Creature> newCreature();
    std::shared_ptr<Placeable> newPlaceable();
    std::shared_ptr<Door> newDoor();
    std::shared_ptr<Waypoint> newWaypoint();
    std::shared_ptr<Trigger> newTrigger();
    std::shared_ptr<Item> newItem();
    std::shared_ptr<Sound> newSound();
    std::shared_ptr<PlaceableCamera> newCamera();
    std::shared_ptr<Encounter> newEncounter();

    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    void setGame(Game &game) { _game = &game; }

    template <class T>
    std::shared_ptr<T> getObjectById(uint32_t id) const {
        return std::dynamic_pointer_cast<T>(getObjectById(id));
    }

private:
    Game *_game {nullptr};
    uint32_t _counter {2}; // ids 0 and 1 are reserved
    std::unordered_map<uint32_t, std::shared_ptr<Object>> _objectById;

    // Services

    ActionFactory &_actionFactory;
    Classes &_classes;
    Combat &_combat;
    FootstepSounds &_footstepSounds;
    Party &_party;
    Portraits &_portraits;
    Reputes &_reputes;
    ScriptRunner &_scriptRunner;
    SoundSets &_soundSets;
    Surfaces &_surfaces;

    audio::AudioFiles &_audioFiles;
    audio::AudioPlayer &_audioPlayer;
    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Models &_models;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::Walkmeshes &_walkmeshes;
    graphics::Window &_window;
    resource::Resources &_resources;
    resource::Strings &_strings;
    resource::TwoDas &_twoDas;
    scene::SceneGraph &_sceneGraph;

    // END Services

    template <class T, class... Args>
    std::shared_ptr<T> newObject(Args &&...args) {
        uint32_t id = _counter++;
        std::shared_ptr<T> object(std::make_shared<T>(id, std::forward<Args>(args)...));
        _objectById.insert(std::make_pair(id, object));
        return move(object);
    }
};

} // namespace game

} // namespace reone
