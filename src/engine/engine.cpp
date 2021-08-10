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

#include "audio/services.h"
#include "common/pathutil.h"
#include "game/kotor.h"
#include "game/tsl.h"
#include "graphics/services.h"
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

/**
 * Encapsulates service initialization.
 */
class Engine : boost::noncopyable {
public:
    Engine(fs::path gamePath, Options options) : _gamePath(move(gamePath)), _options(move(options)) {
    }

    /**
     * Initializes services and starts an instance of Game.
     *
     * @return exit code
     */
    int run() {
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

        unique_ptr<Game> game(newGame(resource, graphics, audio, scene, script));

        return game->run();
    }

private:
    fs::path _gamePath;
    Options _options;

    unique_ptr<Game> newGame(
        ResourceServices &resource,
        GraphicsServices &graphics,
        AudioServices &audio,
        SceneServices &scene,
        ScriptServices &script
    ) {
        GameID gameId = determineGameID();
        switch (gameId) {
            case GameID::KotOR:
                return make_unique<KotOR>(gameId, _gamePath, _options, resource, graphics, audio, scene, script);
            case GameID::TSL:
                return make_unique<TSL>(gameId, _gamePath, _options, resource, graphics, audio, scene, script);
            case GameID::TSL_Steam:
                return make_unique<TSL>(gameId, _gamePath, _options, resource, graphics, audio, scene, script);
        }
    }

    GameID determineGameID() const {
        // If there is no swkotor2 executable, then this is KotOR
        fs::path exePath(getPathIgnoreCase(_gamePath, "swkotor2.exe", false));
        if (exePath.empty()) return GameID::KotOR;

        // If there is a "steam_api.dll" file, then this is a Steam version of TSL
        fs::path dllPath(getPathIgnoreCase(_gamePath, "steam_api.dll", false));
        if (!dllPath.empty()) return GameID::TSL_Steam;

        return GameID::TSL;
    }
};

int runEngine(fs::path gamePath, Options options) {
    return Engine(gamePath, options).run();
}

} // namespace reone
