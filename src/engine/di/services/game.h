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

#include "../../game/action/actionfactory.h"
#include "../../game/combat/combat.h"
#include "../../game/cursors.h"
#include "../../game/d20/classes.h"
#include "../../game/d20/feats.h"
#include "../../game/d20/skills.h"
#include "../../game/d20/spells.h"
#include "../../game/effect/effectfactory.h"
#include "../../game/footstepsounds.h"
#include "../../game/gui/sounds.h"
#include "../../game/object/objectfactory.h"
#include "../../game/party.h"
#include "../../game/portraits.h"
#include "../../game/reputes.h"
#include "../../game/script/routines.h"
#include "../../game/script/runner.h"
#include "../../game/soundsets.h"
#include "../../game/surfaces.h"

namespace reone {

namespace resource {

class ResourceServices;

}

namespace graphics {

class GraphicsServices;

}

namespace audio {

class AudioServices;

}

namespace scene {

class SceneServices;

}

namespace script {

class ScriptServices;

}

namespace game {

class Game;

class GameServices : boost::noncopyable {
public:
    GameServices(
        Game &game,
        resource::ResourceServices &resource,
        graphics::GraphicsServices &graphics,
        audio::AudioServices &audio,
        scene::SceneServices &scene,
        script::ScriptServices &script);

    void init();

    resource::ResourceServices &resource() { return _resource; }
    graphics::GraphicsServices &graphics() { return _graphics; }
    audio::AudioServices &audio() { return _audio; }
    scene::SceneServices &scene() { return _scene; }
    script::ScriptServices &script() { return _script; }

    ActionFactory &actionFactory() { return *_actionFactory; }
    Classes &classes() { return *_classes; }
    Combat &combat() { return *_combat; }
    Cursors &cursors() { return *_cursors; }
    EffectFactory &effectFactory() { return *_effectFactory; }
    Feats &feats() { return *_feats; }
    FootstepSounds &footstepSounds() { return *_footstepSounds; }
    GUISounds &guiSounds() { return *_guiSounds; }
    ObjectFactory &objectFactory() { return *_objectFactory; }
    Party &party() { return *_party; }
    Portraits &portraits() { return *_portraits; }
    Reputes &reputes() { return *_reputes; }
    Routines &routines() { return *_routines; }
    ScriptRunner &scriptRunner() { return *_scriptRunner; }
    SoundSets &soundSets() { return *_soundSets; }
    Skills &skills() { return *_skills; }
    Spells &spells() { return *_spells; }
    Surfaces &surfaces() { return *_surfaces; }

private:
    Game &_game;
    resource::ResourceServices &_resource;
    graphics::GraphicsServices &_graphics;
    audio::AudioServices &_audio;
    scene::SceneServices &_scene;
    script::ScriptServices &_script;
    
    std::unique_ptr<ActionFactory> _actionFactory;
    std::unique_ptr<Classes> _classes;
    std::unique_ptr<Combat> _combat;
    std::unique_ptr<Cursors> _cursors;
    std::unique_ptr<EffectFactory> _effectFactory;
    std::unique_ptr<Feats> _feats;
    std::unique_ptr<FootstepSounds> _footstepSounds;
    std::unique_ptr<GUISounds> _guiSounds;
    std::unique_ptr<ObjectFactory> _objectFactory;
    std::unique_ptr<Party> _party;
    std::unique_ptr<Portraits> _portraits;
    std::unique_ptr<Reputes> _reputes;
    std::unique_ptr<Routines> _routines;
    std::unique_ptr<ScriptRunner> _scriptRunner;
    std::unique_ptr<SoundSets> _soundSets;
    std::unique_ptr<Skills> _skills;
    std::unique_ptr<Spells> _spells;
    std::unique_ptr<Surfaces> _surfaces;
};

} // namespace game

} // namespace reone
