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

using namespace std;

using namespace reone::game;

namespace reone {

void Services::init() {
    _resource = make_unique<ResourceModule>(_gameOptions.gamePath);
    _graphics = make_unique<GraphicsModule>(_gameOptions.graphics, *_resource);
    _audio = make_unique<AudioModule>(_gameOptions.audio, *_resource);
    _scene = make_unique<SceneModule>(_gameOptions.graphics, *_audio, *_graphics);
    _script = make_unique<ScriptModule>(*_resource);
    _game = make_unique<GameModule>(_gameId, _gameOptions, _gameOptions.gamePath, *_resource, *_graphics, *_audio, *_scene, *_script);

    _resource->init();
    _graphics->init();
    _audio->init();
    _scene->init();
    _script->init();
    _game->init();
}

void Services::deinit() {
    _game.reset();
    _script.reset();
    _scene.reset();
    _audio.reset();
    _graphics.reset();
    _resource.reset();
}

} // namespace reone
