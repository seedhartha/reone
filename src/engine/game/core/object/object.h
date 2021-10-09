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

#include "../../../common/timer.h"

#include "../action/action.h"
#include "../types.h"

namespace reone {

namespace resource {

class Resources;
class Strings;

} // namespace resource

namespace graphics {

class Context;
class Meshes;
class Models;
class Shaders;
class Textures;
class Walkmeshes;

} // namespace graphics

namespace audio {

class AudioFiles;
class AudioPlayer;

} // namespace audio

namespace scene {

class SceneGraph;

}

namespace game {

class ActionFactory;
class Classes;
class Combat;
class FootstepSounds;
class Game;
class ObjectFactory;
class Party;
class Portraits;
class Reputes;
class ScriptRunner;
class SoundSets;
class Surfaces;

class Object : public boost::noncopyable {
public:
    virtual ~Object() = default;

    virtual void update(float dt);
    virtual void clearAllActions();

    bool isMinOneHP() const { return _minOneHP; }
    bool isDead() const { return _dead; }
    bool isCommandable() const { return _commandable; }

    uint32_t id() const { return _id; }
    const std::string &tag() const { return _tag; }
    ObjectType type() const { return _type; }
    const std::string &blueprintResRef() const { return _blueprintResRef; }
    const std::string &name() const { return _name; }
    const std::string &conversation() const { return _conversation; }
    bool plotFlag() const { return _plot; }

    void setTag(std::string tag) { _tag = std::move(tag); }
    void setPlotFlag(bool plot) { _plot = plot; }
    void setCommandable(bool commandable) { _commandable = commandable; }

    // Hit Points

    // Base maximum hit points, not considering any bonuses.
    int hitPoints() const { return _hitPoints; }

    // Maximum hit points, after considering all bonuses and penalties.
    int maxHitPoints() const { return _maxHitPoints; }

    // Current hit points, not counting any bonuses.
    int currentHitPoints() const { return _currentHitPoints; }

    void setMinOneHP(bool minOneHP) { _minOneHP = minOneHP; }
    void setMaxHitPoints(int maxHitPoints) { _maxHitPoints = maxHitPoints; }
    void setCurrentHitPoints(int hitPoints) { _currentHitPoints = hitPoints; }

    // END Hit Points

    // Actions

    void addAction(std::unique_ptr<Action> action);
    void addActionOnTop(std::unique_ptr<Action> action);
    void delayAction(std::unique_ptr<Action> action, float seconds);

    bool hasUserActionsPending() const;

    std::shared_ptr<Action> getCurrentAction() const;

    const std::deque<std::shared_ptr<Action>> &actions() const { return _actions; }

    // END Actions

    // Local variables

    bool getLocalBoolean(int index) const;
    int getLocalNumber(int index) const;

    void setLocalBoolean(int index, bool value);
    void setLocalNumber(int index, int value);

    // END Local variables

    // Scripts

    const std::string &getOnHeartbeat() const { return _onHeartbeat; }
    const std::string &getOnUserDefined() const { return _onUserDefined; }

    // END Scripts

protected:
    struct DelayedAction {
        std::unique_ptr<Action> action;
        Timer timer;
    };

    Game *_game;

    uint32_t _id {0};
    std::string _tag;
    ObjectType _type {ObjectType::Invalid};
    std::string _blueprintResRef;
    std::string _name;
    std::string _conversation;
    bool _minOneHP {false};
    int _hitPoints {0};
    int _maxHitPoints {0};
    int _currentHitPoints {0};
    bool _dead {false};
    bool _plot {false};
    bool _commandable {true};
    bool _autoRemoveKey {false};
    bool _interruptable {false};

    // Services

    ActionFactory &_actionFactory;
    Classes &_classes;
    Combat &_combat;
    FootstepSounds &_footstepSounds;
    ObjectFactory &_objectFactory;
    Party &_party;
    Portraits &_portraits;
    Reputes &_reputes;
    ScriptRunner &_scriptRunner;
    SoundSets &_soundSets;
    Surfaces &_surfaces;

    audio::AudioFiles &_audioFiles;
    audio::AudioPlayer &_audioPlayer;
    graphics::Context &_context;
    graphics::Meshes &_meshes;
    graphics::Models &_models;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::Walkmeshes &_walkmeshes;
    resource::Resources &_resources;
    resource::Strings &_strings;
    scene::SceneGraph &_sceneGraph;

    // END Services

    // Actions

    std::deque<std::shared_ptr<Action>> _actions;
    std::vector<DelayedAction> _delayed;

    // END Actions

    // Local variables

    std::map<int, bool> _localBooleans;
    std::map<int, int> _localNumbers;

    // END Local variables

    // Scripts

    std::string _onDeath;
    std::string _onHeartbeat;
    std::string _onUserDefined;

    // END Scripts

    Object(
        uint32_t id,
        ObjectType type,
        Game *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        FootstepSounds &footstepSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        resource::Resources &resources,
        resource::Strings &strings,
        scene::SceneGraph &sceneGraph) :
        _id(id),
        _type(type),
        _game(game),
        _actionFactory(actionFactory),
        _classes(classes),
        _combat(combat),
        _footstepSounds(footstepSounds),
        _objectFactory(objectFactory),
        _party(party),
        _portraits(portraits),
        _reputes(reputes),
        _scriptRunner(scriptRunner),
        _soundSets(soundSets),
        _surfaces(surfaces),
        _audioFiles(audioFiles),
        _audioPlayer(audioPlayer),
        _context(context),
        _meshes(meshes),
        _models(models),
        _shaders(shaders),
        _textures(textures),
        _walkmeshes(walkmeshes),
        _resources(resources),
        _strings(strings),
        _sceneGraph(sceneGraph) {
    }

    // Actions

    void updateActions(float dt);
    void removeCompletedActions();
    void updateDelayedActions(float dt);

    void executeActions(float dt);

    // END Actions
};

} // namespace game

} // namespace reone
