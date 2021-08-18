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

#include "services.h"

#include "game.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

GameServices::GameServices(
    Game &game,
    ResourceServices &resource,
    GraphicsServices &graphics,
    AudioServices &audio,
    SceneServices &scene,
    ScriptServices &script
) :
    _game(game),
    _resource(resource),
    _graphics(graphics),
    _audio(audio),
    _scene(scene),
    _script(script) {
}

void GameServices::init() {
    _surfaces = make_unique<Surfaces>(_resource.resources());
    _surfaces->init();

    _cursors = make_unique<Cursors>(_game.id(), _graphics, _resource);
    _soundSets = make_unique<SoundSets>(_audio.files(), _resource);
    _footstepSounds = make_unique<FootstepSounds>(_audio.files(), _resource.resources());

    _guiSounds = make_unique<GUISounds>(_audio.files(), _resource.resources());
    _guiSounds->init();

    _routines = make_unique<Routines>(_game);
    _routines->init();

    _scriptRunner = make_unique<ScriptRunner>(*_routines, _script.scripts());

    _reputes = make_unique<Reputes>(_resource.resources());
    _reputes->init();

    _skills = make_unique<Skills>(_graphics, _resource);
    _skills->init();

    _feats = make_unique<Feats>(_graphics.textures(), _resource);
    _feats->init();

    _spells = make_unique<Spells>(_graphics.textures(), _resource);
    _spells->init();

    _classes = make_unique<Classes>(_resource);

    _portraits = make_unique<Portraits>(_graphics.textures(), _resource.resources());
    _portraits->init();

    _objectFactory = make_unique<ObjectFactory>(_game, _scene.graph());

    _party = make_unique<Party>(_game);
    _combat = make_unique<Combat>(_game, _scene);
    _actionFactory = make_unique<ActionFactory>(_game);
}

} // namespace game

} // namespace reone
