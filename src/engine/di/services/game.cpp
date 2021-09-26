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
    _game = newGame();
    _game->initResourceProviders();

    _surfaces = make_unique<Surfaces>(_resource.resources());
    _surfaces->init();

    _cursors = make_unique<Cursors>(_gameId, _graphics.context(), _graphics.meshes(), _graphics.shaders(), _graphics.window(), _resource.resources());

    _soundSets = make_unique<SoundSets>(_audio.files(), _resource.resources(), _resource.strings());

    _footstepSounds = make_unique<FootstepSounds>(_audio.files(), _resource.resources());

    _guiSounds = make_unique<GUISounds>(_audio.files(), _resource.resources());
    _guiSounds->init();

    _routines = make_unique<Routines>(*_game);

    _scriptRunner = make_unique<ScriptRunner>(*_routines, _script.scripts());

    _reputes = make_unique<Reputes>(_resource.resources());
    _reputes->init();

    _skills = make_unique<Skills>(_graphics.textures(), _resource.resources(), _resource.strings());
    _skills->init();

    _feats = make_unique<Feats>(_graphics.textures(), _resource.resources(), _resource.strings());
    _feats->init();

    _spells = make_unique<Spells>(_graphics.textures(), _resource.resources(), _resource.strings());
    _spells->init();

    _classes = make_unique<Classes>(_resource.resources(), _resource.strings());

    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.resources());
    _portraits->init();

    _objectFactory = make_unique<ObjectFactory>(*_game, _scene.graph());

    _party = make_unique<Party>(*_game);

    _combat = make_unique<Combat>(*_game, _scene.graph());

    _actionFactory = make_unique<ActionFactory>(*_game);

    _effectFactory = make_unique<EffectFactory>();

    _routineRegistrar = newRoutineRegistrar();
    _routineRegistrar->invoke();

    _game->init();
}

unique_ptr<Game> GameServices::newGame() {
    switch (_gameId) {
        case GameID::KotOR:
            return make_unique<KotOR>(_gamePath, _gameOptions, *this, _resource, _graphics, _audio, _scene, _script);
        case GameID::TSL:
            return make_unique<TSL>(_gamePath, _gameOptions, *this, _resource, _graphics, _audio, _scene, _script);
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
