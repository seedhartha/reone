/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/camerastyles.h"
#include "reone/game/cursors.h"
#include "reone/game/d20/classes.h"
#include "reone/game/d20/feats.h"
#include "reone/game/d20/skills.h"
#include "reone/game/d20/spells.h"
#include "reone/game/dialogs.h"
#include "reone/game/footstepsounds.h"
#include "reone/game/game.h"
#include "reone/game/guisounds.h"
#include "reone/game/layouts.h"
#include "reone/game/paths.h"
#include "reone/game/portraits.h"
#include "reone/game/reputes.h"
#include "reone/game/resourcelayout.h"
#include "reone/game/services.h"
#include "reone/game/soundsets.h"
#include "reone/game/surfaces.h"
#include "reone/game/types.h"
#include "reone/game/visibilities.h"

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

    std::unique_ptr<game::CameraStyles> _cameraStyles;
    std::unique_ptr<game::Classes> _classes;
    std::unique_ptr<game::Cursors> _cursors;
    std::unique_ptr<game::Dialogs> _dialogs;
    std::unique_ptr<game::Feats> _feats;
    std::unique_ptr<game::FootstepSounds> _footstepSounds;
    std::unique_ptr<game::GUISounds> _guiSounds;
    std::unique_ptr<game::Layouts> _layouts;
    std::unique_ptr<game::Paths> _paths;
    std::unique_ptr<game::Portraits> _portraits;
    std::unique_ptr<game::Reputes> _reputes;
    std::unique_ptr<game::ResourceLayout> _resourceLayout;
    std::unique_ptr<game::Skills> _skills;
    std::unique_ptr<game::SoundSets> _soundSets;
    std::unique_ptr<game::Spells> _spells;
    std::unique_ptr<game::Surfaces> _surfaces;
    std::unique_ptr<game::Visibilities> _visibilities;

    std::unique_ptr<game::GameServices> _services;
};

} // namespace engine

} // namespace reone
