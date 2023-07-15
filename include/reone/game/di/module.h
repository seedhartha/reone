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

#include "reone/audio/di/module.h"
#include "reone/graphics/di/module.h"
#include "reone/resource/di/module.h"
#include "reone/scene/di/module.h"
#include "reone/script/di/module.h"

#include "../camerastyles.h"
#include "../cursors.h"
#include "../d20/classes.h"
#include "../d20/feats.h"
#include "../d20/skills.h"
#include "../d20/spells.h"
#include "../dialogs.h"
#include "../footstepsounds.h"
#include "../gui/sounds.h"
#include "../layouts.h"
#include "../options.h"
#include "../paths.h"
#include "../portraits.h"
#include "../reputes.h"
#include "../resourcedirector.h"
#include "../soundsets.h"
#include "../surfaces.h"
#include "../types.h"
#include "../visibilities.h"

#include "services.h"

namespace reone {

namespace game {

class GameModule : boost::noncopyable {
public:
    GameModule(
        GameID gameId,
        OptionsView &options,
        resource::ResourceModule &resource,
        graphics::GraphicsModule &graphics,
        audio::AudioModule &audio,
        scene::SceneModule &scene,
        script::ScriptModule &script) :
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

    GameServices &services() { return *_services; }

private:
    GameID _gameId;
    OptionsView &_options;

    resource::ResourceModule &_resource;
    graphics::GraphicsModule &_graphics;
    audio::AudioModule &_audio;
    scene::SceneModule &_scene;
    script::ScriptModule &_script;

    std::unique_ptr<CameraStyles> _cameraStyles;
    std::unique_ptr<Classes> _classes;
    std::unique_ptr<Cursors> _cursors;
    std::unique_ptr<Dialogs> _dialogs;
    std::unique_ptr<Feats> _feats;
    std::unique_ptr<FootstepSounds> _footstepSounds;
    std::unique_ptr<GUISounds> _guiSounds;
    std::unique_ptr<Layouts> _layouts;
    std::unique_ptr<Paths> _paths;
    std::unique_ptr<Portraits> _portraits;
    std::unique_ptr<Reputes> _reputes;
    std::unique_ptr<ResourceDirector> _resourceDirector;
    std::unique_ptr<Skills> _skills;
    std::unique_ptr<SoundSets> _soundSets;
    std::unique_ptr<Spells> _spells;
    std::unique_ptr<Surfaces> _surfaces;
    std::unique_ptr<Visibilities> _visibilities;

    std::unique_ptr<GameServices> _services;
};

} // namespace game

} // namespace reone
