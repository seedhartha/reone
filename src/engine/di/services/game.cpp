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

#include "../../game/kotor.h"
#include "../../game/script/routine/registrar/kotor.h"
#include "../../game/script/routine/registrar/tsl.h"
#include "../../game/tsl.h"

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

void GameServices::init() {
    _surfaces = make_unique<Surfaces>(_resource.resources());
    _cursors = make_unique<Cursors>(_gameId, _graphics.context(), _graphics.meshes(), _graphics.shaders(), _graphics.window(), _resource.resources());
    _soundSets = make_unique<SoundSets>(_audio.files(), _resource.resources(), _resource.strings());
    _footstepSounds = make_unique<FootstepSounds>(_audio.files(), _resource.resources());
    _guiSounds = make_unique<GUISounds>(_audio.files(), _resource.resources());
    _scriptRunner = make_unique<ScriptRunner>(_script.scripts());
    _reputes = make_unique<Reputes>(_resource.resources());
    _skills = make_unique<Skills>(_graphics.textures(), _resource.resources(), _resource.strings());
    _feats = make_unique<Feats>(_graphics.textures(), _resource.resources(), _resource.strings());
    _spells = make_unique<Spells>(_graphics.textures(), _resource.resources(), _resource.strings());
    _classes = make_unique<Classes>(_resource.resources(), _resource.strings());
    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.resources());
    _actionFactory = make_unique<ActionFactory>();
    _party = make_unique<Party>();
    _combat = make_unique<Combat>(*_effectFactory, _scene.graph());
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
        _audio.files(),
        _audio.player(),
        _graphics.context(),
        _graphics.meshes(),
        _graphics.models(),
        _graphics.shaders(),
        _graphics.textures(),
        _graphics.walkmeshes(),
        _graphics.window(),
        _resource.resources(),
        _resource.strings(),
        _scene.graph());
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

unique_ptr<Game> GameServices::newGame() {
    switch (_gameId) {
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
            _audio.files(),
            _audio.player(),
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
            _scene.graph(),
            _scene.worldRenderPipeline(),
            _script.scripts(),
            _resource.resources(),
            _resource.strings());

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
            _audio.files(),
            _audio.player(),
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
            _scene.graph(),
            _scene.worldRenderPipeline(),
            _script.scripts(),
            _resource.resources(),
            _resource.strings());

    default:
        throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(_gameId)));
    }
}

unique_ptr<RoutineRegistrar> GameServices::newRoutineRegistrar() {
    switch (_gameId) {
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
