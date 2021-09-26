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

#include "container.h"

using namespace std;

using namespace reone::game;

namespace reone {

namespace di {

void Container::init() {
    _commonServices = make_unique<CommonServices>();
    _resourceServices = make_unique<ResourceServices>(_gameOptions.gamePath);
    _graphicsServices = make_unique<GraphicsServices>(_gameOptions.graphics, *_resourceServices);
    _audioServices = make_unique<AudioServices>(_gameOptions.audio, *_resourceServices);
    _sceneServices = make_unique<SceneServices>(_gameOptions.graphics, *_graphicsServices);
    _scriptServices = make_unique<ScriptServices>(*_resourceServices);
    _gameServices = make_unique<GameServices>(_gameId, _gameOptions, _gameOptions.gamePath, *_resourceServices, *_graphicsServices, *_audioServices, *_sceneServices, *_scriptServices);

    _commonServices->init();
    _resourceServices->init();
    _graphicsServices->init();
    _audioServices->init();
    _sceneServices->init();
    _scriptServices->init();
    _gameServices->init();
}

Game &Container::getGame() {
    return _gameServices->game();
}

} // namespace di

} // namespace reone