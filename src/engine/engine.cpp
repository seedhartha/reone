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

#include "engine.h"

#include "audio/player.h"
#include "audio/services.h"
#include "game/game.h"
#include "graphics/services.h"
#include "resource/resourceprovider.h"
#include "resource/services.h"
#include "scene/services.h"
#include "script/services.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

Engine::Engine(fs::path gamePath, Options options) : _gamePath(move(gamePath)), _options(move(options)) {
}

int Engine::run() {
    ResourceServices resource(_gamePath);
    resource.init();

    GraphicsServices graphics(_options.graphics, resource);
    graphics.init();

    AudioServices audio(_options.audio, resource);
    audio.init();

    SceneServices scene(_options.graphics, graphics);
    scene.init();

    ScriptServices script(resource);
    script.init();

    Game game(
        _gamePath,
        _options,
        resource, graphics, audio, scene, script);

    return game.run();
}

} // namespace reone
