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

#include "factory.h"

#include "../../../common/collectionutil.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

shared_ptr<Module> ObjectFactory::newModule() {
    return newObject<Module>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Area> ObjectFactory::newArea() {
    return newObject<Area>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Creature> ObjectFactory::newCreature() {
    return newObject<Creature>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Placeable> ObjectFactory::newPlaceable() {
    return newObject<Placeable>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Door> ObjectFactory::newDoor() {
    return newObject<Door>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Waypoint> ObjectFactory::newWaypoint() {
    return newObject<Waypoint>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Trigger> ObjectFactory::newTrigger() {
    return newObject<Trigger>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Item> ObjectFactory::newItem() {
    return newObject<Item>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Sound> ObjectFactory::newSound() {
    return newObject<Sound>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<PlaceableCamera> ObjectFactory::newCamera() {
    return newObject<PlaceableCamera>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Encounter> ObjectFactory::newEncounter() {
    return newObject<Encounter>(
        _game,
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        *this,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _gffs,
        _resources,
        _strings,
        _twoDas,
        _sceneGraph);
}

shared_ptr<Object> ObjectFactory::getObjectById(uint32_t id) const {
    return getFromLookupOrNull(_objectById, id);
}

} // namespace game

} // namespace reone
