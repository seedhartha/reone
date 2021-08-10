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
#include "graphics/context.h"
#include "graphics/features.h"
#include "graphics/fonts.h"
#include "graphics/lip/lips.h"
#include "graphics/materials.h"
#include "graphics/mesh/meshes.h"
#include "graphics/model/models.h"
#include "graphics/pbribl.h"
#include "graphics/renderbuffer.h"
#include "graphics/services.h"
#include "graphics/walkmesh/walkmeshes.h"
#include "graphics/window.h"
#include "resource/resourceprovider.h"
#include "resource/services.h"
#include "scene/services.h"
#include "scene/pipeline/world.h"
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

        return Game(
            _gamePath,
            _options,
            resource, graphics, audio, scene, script).run();
    }

private:
    fs::path _gamePath;
    Options _options;
};

int runEngine(fs::path gamePath, Options options) {
    return Engine(gamePath, options).run();
}

} // namespace reone
