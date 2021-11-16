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

#include "game.h"

#include "../../game/kotor/kotor.h"
#include "../../game/kotor/routine/registrar.h"
#include "../../game/limbo/limbo.h"
#include "../../game/limbo/routine/registrar.h"
#include "../../game/tsl/routine/registrar.h"
#include "../../game/tsl/tsl.h"

#include "audio.h"
#include "graphics.h"
#include "resource.h"
#include "scene.h"
#include "script.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace di {

void GameModule::init() {
    _surfaces = make_unique<Surfaces>(_resource.twoDas());
    _cursors = make_unique<Cursors>(_graphics.context(), _graphics.meshes(), _graphics.shaders(), _graphics.window(), _resource.resources());
    _soundSets = make_unique<SoundSets>(_audio.audioFiles(), _resource.resources(), _resource.strings());
    _footstepSounds = make_unique<FootstepSounds>(_audio.audioFiles(), _resource.twoDas());
    _guiSounds = make_unique<GUISounds>(_audio.audioFiles(), _resource.twoDas());
    _scriptRunner = make_unique<ScriptRunner>(_script.scripts());
    _reputes = make_unique<Reputes>(_resource.twoDas());
    _skills = make_unique<Skills>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _feats = make_unique<Feats>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _spells = make_unique<Spells>(_graphics.textures(), _resource.strings(), _resource.twoDas());
    _classes = make_unique<Classes>(_resource.strings(), _resource.twoDas());
    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.twoDas());
    _actionFactory = make_unique<ActionFactory>();
    _party = make_unique<Party>();
    _combat = make_unique<Combat>(*_effectFactory, _scene.sceneGraph());
    _objectFactory = make_unique<ObjectFactory>(
        *_actionFactory,
        *_classes,
        *_combat,
        *_footstepSounds,
        *_party,
        *_portraits,
        *_reputes,
        *_scriptRunner,
        *_soundSets,
        *_surfaces,
        _audio.audioFiles(),
        _audio.audioPlayer(),
        _graphics.context(),
        _graphics.meshes(),
        _graphics.models(),
        _graphics.shaders(),
        _graphics.textures(),
        _graphics.walkmeshes(),
        _graphics.window(),
        _resource.resources(),
        _resource.strings(),
        _resource.twoDas(),
        _scene.sceneGraph());
    _effectFactory = make_unique<EffectFactory>();
    _routines = make_unique<Routines>(*_actionFactory, *_combat, *_effectFactory, *_party, *_reputes, *_scriptRunner, _resource.strings());
    _routineRegistrar = newRoutineRegistrar();
    _game = newGame();

    _scriptRunner->setRoutines(*_routines);
    _actionFactory->setGame(*_game);
    _party->setGame(*_game);
    _combat->setGame(*_game);
    _objectFactory->setGame(*_game);
    _routines->setGame(*_game);

    _game->initResourceProviders();
    _surfaces->init();
    _guiSounds->init();
    _reputes->init();
    _skills->init();
    _feats->init();
    _spells->init();
    _portraits->init();
    _routineRegistrar->invoke();
    _game->init();
}

unique_ptr<Game> GameModule::newGame() {
    switch (_gameId) {
    case GameID::Limbo:
        return make_unique<Limbo>(
            _gamePath,
            _gameOptions,
            *_actionFactory,
            *_classes,
            *_combat,
            *_cursors,
            *_effectFactory,
            *_feats,
            *_footstepSounds,
            *_guiSounds,
            *_objectFactory,
            *_party,
            *_portraits,
            *_reputes,
            *_scriptRunner,
            *_skills,
            *_soundSets,
            *_surfaces,
            _audio.audioFiles(),
            _audio.audioPlayer(),
            _graphics.context(),
            _graphics.features(),
            _graphics.fonts(),
            _graphics.lips(),
            _graphics.materials(),
            _graphics.meshes(),
            _graphics.models(),
            _graphics.pbrIbl(),
            _graphics.shaders(),
            _graphics.textures(),
            _graphics.walkmeshes(),
            _graphics.window(),
            _scene.sceneGraph(),
            _scene.worldRenderPipeline(),
            _script.scripts(),
            _resource.resources(),
            _resource.strings(),
            _resource.twoDas());

    case GameID::KotOR:
        return make_unique<KotOR>(
            _gamePath,
            _gameOptions,
            *_actionFactory,
            *_classes,
            *_combat,
            *_cursors,
            *_effectFactory,
            *_feats,
            *_footstepSounds,
            *_guiSounds,
            *_objectFactory,
            *_party,
            *_portraits,
            *_reputes,
            *_scriptRunner,
            *_skills,
            *_soundSets,
            *_surfaces,
            _audio.audioFiles(),
            _audio.audioPlayer(),
            _graphics.context(),
            _graphics.features(),
            _graphics.fonts(),
            _graphics.lips(),
            _graphics.materials(),
            _graphics.meshes(),
            _graphics.models(),
            _graphics.pbrIbl(),
            _graphics.shaders(),
            _graphics.textures(),
            _graphics.walkmeshes(),
            _graphics.window(),
            _scene.sceneGraph(),
            _scene.worldRenderPipeline(),
            _script.scripts(),
            _resource.resources(),
            _resource.strings(),
            _resource.twoDas());

    case GameID::TSL:
        return make_unique<TSL>(
            _gamePath,
            _gameOptions,
            *_actionFactory,
            *_classes,
            *_combat,
            *_cursors,
            *_effectFactory,
            *_feats,
            *_footstepSounds,
            *_guiSounds,
            *_objectFactory,
            *_party,
            *_portraits,
            *_reputes,
            *_scriptRunner,
            *_skills,
            *_soundSets,
            *_surfaces,
            _audio.audioFiles(),
            _audio.audioPlayer(),
            _graphics.context(),
            _graphics.features(),
            _graphics.fonts(),
            _graphics.lips(),
            _graphics.materials(),
            _graphics.meshes(),
            _graphics.models(),
            _graphics.pbrIbl(),
            _graphics.shaders(),
            _graphics.textures(),
            _graphics.walkmeshes(),
            _graphics.window(),
            _scene.sceneGraph(),
            _scene.worldRenderPipeline(),
            _script.scripts(),
            _resource.resources(),
            _resource.strings(),
            _resource.twoDas());

    default:
        throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(_gameId)));
    }
}

unique_ptr<RoutineRegistrar> GameModule::newRoutineRegistrar() {
    switch (_gameId) {
    case GameID::Limbo:
        return make_unique<LimboRoutineRegistrar>(*_routines);
    case GameID::KotOR:
        return make_unique<KotORRoutineRegistrar>(*_routines);
    case GameID::TSL:
        return make_unique<TSLRoutineRegistrar>(*_routines);
    default:
        throw invalid_argument("Unsupported game ID: " + to_string(static_cast<int>(_gameId)));
    }
}

} // namespace di

} // namespace reone
