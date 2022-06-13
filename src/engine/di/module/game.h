/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#pragma once

#include "../../game/astar.h"
#include "../../game/camerastyles.h"
#include "../../game/cursors.h"
#include "../../game/footstepsounds.h"
#include "../../game/game.h"
#include "../../game/guisounds.h"
#include "../../game/layouts.h"
#include "../../game/paths.h"
#include "../../game/portraits.h"
#include "../../game/resourcelayout.h"
#include "../../game/services.h"
#include "../../game/soundsets.h"
#include "../../game/surfaces.h"
#include "../../game/types.h"
#include "../../game/visibilities.h"

namespace reone {

namespace engine {

class AudioModule;
class GraphicsModule;
class ResourceModule;
class SceneModule;
class ScriptModule;

class GameModule : boost::noncopyable {
public:
    GameModule(
        game::GameID gameId,
        game::OptionsView &options,
        ResourceModule &resource,
        GraphicsModule &graphics,
        AudioModule &audio,
        SceneModule &scene,
        ScriptModule &script) :
        _gameId(gameId),
        _options(options),
        _resource(resource),
        _graphics(graphics),
        _audio(audio),
        _scene(scene),
        _script(script) {
    }

    ~GameModule() { deinit(); }

    void init();
    void deinit();

    game::GameServices &services() { return *_services; }

private:
    game::GameID _gameId;
    game::OptionsView &_options;

    ResourceModule &_resource;
    GraphicsModule &_graphics;
    AudioModule &_audio;
    SceneModule &_scene;
    ScriptModule &_script;

    std::unique_ptr<game::AStar> _aStar;
    std::unique_ptr<game::CameraStyles> _cameraStyles;
    std::unique_ptr<game::Cursors> _cursors;
    std::unique_ptr<game::FootstepSounds> _footstepSounds;
    std::unique_ptr<game::GUISounds> _guiSounds;
    std::unique_ptr<game::Layouts> _layouts;
    std::unique_ptr<game::Paths> _paths;
    std::unique_ptr<game::Portraits> _portraits;
    std::unique_ptr<game::ResourceLayout> _resourceLayout;
    std::unique_ptr<game::SoundSets> _soundSets;
    std::unique_ptr<game::Surfaces> _surfaces;
    std::unique_ptr<game::Visibilities> _visibilities;

    std::unique_ptr<game::GameServices> _services;
};

} // namespace engine

} // namespace reone
