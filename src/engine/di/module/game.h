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

#pragma once

#include "../../game/core/action/factory.h"
#include "../../game/core/combat.h"
#include "../../game/core/cursors.h"
#include "../../game/core/d20/classes.h"
#include "../../game/core/d20/feats.h"
#include "../../game/core/d20/skills.h"
#include "../../game/core/d20/spells.h"
#include "../../game/core/effect/factory.h"
#include "../../game/core/footstepsounds.h"
#include "../../game/core/game.h"
#include "../../game/core/object/factory.h"
#include "../../game/core/party.h"
#include "../../game/core/portraits.h"
#include "../../game/core/reputes.h"
#include "../../game/core/script/routine/registrar.h"
#include "../../game/core/script/routine/routines.h"
#include "../../game/core/script/runner.h"
#include "../../game/core/soundsets.h"
#include "../../game/core/surfaces.h"
#include "../../game/kotor/gui/sounds.h"
#include "../../game/types.h"

namespace reone {

namespace di {

class AudioModule;
class GraphicsModule;
class ResourceModule;
class SceneModule;
class ScriptModule;

class GameModule : boost::noncopyable {
public:
    GameModule(
        game::GameID gameId,
        game::Options gameOptions,
        boost::filesystem::path gamePath,
        ResourceModule &resource,
        GraphicsModule &graphics,
        AudioModule &audio,
        SceneModule &scene,
        ScriptModule &script) :
        _gameId(gameId),
        _gamePath(std::move(gamePath)),
        _gameOptions(std::move(gameOptions)),
        _resource(resource),
        _graphics(graphics),
        _audio(audio),
        _scene(scene),
        _script(script) {
    }

    void init();

    game::ActionFactory &actionFactory() { return *_actionFactory; }
    game::Classes &classes() { return *_classes; }
    game::Combat &combat() { return *_combat; }
    game::Cursors &cursors() { return *_cursors; }
    game::EffectFactory &effectFactory() { return *_effectFactory; }
    game::Feats &feats() { return *_feats; }
    game::FootstepSounds &footstepSounds() { return *_footstepSounds; }
    game::Game &game() { return *_game; }
    game::GUISounds &guiSounds() { return *_guiSounds; }
    game::ObjectFactory &objectFactory() { return *_objectFactory; }
    game::Party &party() { return *_party; }
    game::Portraits &portraits() { return *_portraits; }
    game::Reputes &reputes() { return *_reputes; }
    game::Routines &routines() { return *_routines; }
    game::RoutineRegistrar &routineRegistrar() { return *_routineRegistrar; }
    game::ScriptRunner &scriptRunner() { return *_scriptRunner; }
    game::SoundSets &soundSets() { return *_soundSets; }
    game::Skills &skills() { return *_skills; }
    game::Spells &spells() { return *_spells; }
    game::Surfaces &surfaces() { return *_surfaces; }

private:
    game::GameID _gameId;
    game::Options _gameOptions;
    boost::filesystem::path _gamePath;

    ResourceModule &_resource;
    GraphicsModule &_graphics;
    AudioModule &_audio;
    SceneModule &_scene;
    ScriptModule &_script;

    std::unique_ptr<game::ActionFactory> _actionFactory;
    std::unique_ptr<game::Classes> _classes;
    std::unique_ptr<game::Combat> _combat;
    std::unique_ptr<game::Cursors> _cursors;
    std::unique_ptr<game::EffectFactory> _effectFactory;
    std::unique_ptr<game::Feats> _feats;
    std::unique_ptr<game::FootstepSounds> _footstepSounds;
    std::unique_ptr<game::Game> _game;
    std::unique_ptr<game::GUISounds> _guiSounds;
    std::unique_ptr<game::ObjectFactory> _objectFactory;
    std::unique_ptr<game::Party> _party;
    std::unique_ptr<game::Portraits> _portraits;
    std::unique_ptr<game::Reputes> _reputes;
    std::unique_ptr<game::Routines> _routines;
    std::unique_ptr<game::RoutineRegistrar> _routineRegistrar;
    std::unique_ptr<game::ScriptRunner> _scriptRunner;
    std::unique_ptr<game::SoundSets> _soundSets;
    std::unique_ptr<game::Skills> _skills;
    std::unique_ptr<game::Spells> _spells;
    std::unique_ptr<game::Surfaces> _surfaces;

    std::unique_ptr<game::Game> newGame();
    std::unique_ptr<game::RoutineRegistrar> newRoutineRegistrar();
};

} // namespace di

} // namespace reone
