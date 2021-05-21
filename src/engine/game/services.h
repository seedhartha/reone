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

#include <boost/noncopyable.hpp>

namespace reone {

namespace game {

class Surfaces;
class Cursors;
class SoundSets;
class FootstepSounds;
class GUISounds;
class Routines;
class ScriptRunner;
class Reputes;
class Feats;
class Spells;
class Classes;
class Portraits;
class ObjectFactory;
class Party;
class Combat;

class GameServices : boost::noncopyable {
public:
    GameServices(
        Surfaces &surfaces,
        Cursors &cursors,
        SoundSets &soundSets,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        Routines &routines,
        ScriptRunner &scriptRunner,
        Reputes &reputes,
        Feats &feats,
        Spells &spells,
        Classes &classes,
        Portraits &portraits,
        ObjectFactory &objectFactory,
        Party &party,
        Combat &combat
    ) :
        _surfaces(surfaces),
        _cursors(cursors),
        _soundSets(soundSets),
        _footstepSounds(footstepSounds),
        _guiSounds(guiSounds),
        _routines(routines),
        _scriptRunner(scriptRunner),
        _reputes(reputes),
        _feats(feats),
        _spells(spells),
        _classes(classes),
        _portraits(portraits),
        _objectFactory(objectFactory),
        _party(party),
        _combat(combat) {
    }

    Surfaces &surfaces() { return _surfaces; }
    Cursors &cursors() { return _cursors; }
    SoundSets &soundSets() { return _soundSets; }
    FootstepSounds &footstepSounds() { return _footstepSounds; }
    SoundSets &soundSets() { return _soundSets; }
    FootstepSounds &footstepSounds() { return _footstepSounds; }
    GUISounds &guiSounds() { return _guiSounds; }
    Routines &routines() { return _routines; }
    ScriptRunner &scriptRunner() { return _scriptRunner; }
    Reputes &reputes() { return _reputes; }
    Feats &feats() { return _feats; }
    Spells &spells() { return _spells; }
    Classes &classes() { return _classes; }
    Portraits &portraits() { return _portraits; }
    ObjectFactory &objectFactory() { return _objectFactory; }
    Party &party() { return _party; }
    Combat &combat() { return _combat; }

private:
    Surfaces &_surfaces;
    Cursors &_cursors;
    SoundSets &_soundSets;
    FootstepSounds &_footstepSounds;
    GUISounds &_guiSounds;
    Routines &_routines;
    ScriptRunner &_scriptRunner;
    Reputes &_reputes;
    Feats &_feats;
    Spells &_spells;
    Classes &_classes;
    Portraits &_portraits;
    ObjectFactory &_objectFactory;
    Party &_party;
    Combat &_combat;
};

} // namespace game

} // namespace reone
