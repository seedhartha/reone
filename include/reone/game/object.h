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

#include "reone/scene/animproperties.h"
#include "reone/scene/graph.h"
#include "reone/scene/node.h"
#include "reone/scene/user.h"
#include "reone/system/timer.h"

#include "action.h"
#include "action/playanimation.h"
#include "effect.h"
#include "types.h"

namespace reone {

namespace game {

struct ServicesView;

class Action;
class Game;
class Item;
class Room;

class Object : public scene::IUser, boost::noncopyable {
public:
    virtual ~Object() = default;

    virtual void update(float dt);
    virtual void die();

    void face(const Object &other);
    void face(const glm::vec3 &point);
    void faceAwayFrom(const Object &other);

    bool contains(const glm::vec3 &point) const;

    virtual bool isSelectable() const;
    bool isOpen() const { return _open; }
    bool isInLineOfSight(const Object &other, float fov) const;

    bool isMinOneHP() const { return _minOneHP; }
    bool isDead() const { return _dead; }
    bool isCommandable() const { return _commandable; }

    float getDistanceTo(const glm::vec2 &point) const;
    float getSquareDistanceTo(const glm::vec2 &point) const;
    float getDistanceTo(const glm::vec3 &point) const;
    float getSquareDistanceTo(const glm::vec3 &point) const;
    float getDistanceTo(const Object &other) const;
    float getSquareDistanceTo(const Object &other) const;

    virtual glm::vec3 getSelectablePosition() const;
    float getFacing() const { return glm::eulerAngles(_orientation).z; }

    uint32_t id() const { return _id; }
    const std::string &tag() const { return _tag; }
    ObjectType type() const { return _type; }
    const std::string &blueprintResRef() const { return _blueprintResRef; }
    const std::string &name() const { return _name; }
    const std::string &conversation() const { return _conversation; }
    bool plotFlag() const { return _plot; }

    Room *room() const { return _room; }
    const glm::vec3 &position() const { return _position; }
    const glm::mat4 &transform() const { return _transform; }
    bool visible() const { return _visible; }
    std::shared_ptr<scene::SceneNode> sceneNode() const { return _sceneNode; }

    void setTag(std::string tag) { _tag = std::move(tag); }
    void setPlotFlag(bool plot) { _plot = plot; }
    void setCommandable(bool commandable) { _commandable = commandable; }

    void setRoom(Room *room);
    void setPosition(const glm::vec3 &position);
    void setFacing(float facing);
    void setVisible(bool visible);

    // Animation

    virtual void playAnimation(AnimationType type, scene::AnimationProperties properties = scene::AnimationProperties());

    virtual std::string getAnimationName(AnimationType type) const { return ""; }
    virtual std::string getActiveAnimationName() const { return ""; };

    // END Animation

    // Inventory

    std::shared_ptr<Item> addItem(const std::string &resRef, int stackSize = 1, bool dropable = true);
    void addItem(const std::shared_ptr<Item> &item);
    bool removeItem(const std::shared_ptr<Item> &item, bool &last);
    void moveDropableItemsTo(Object &other);

    std::shared_ptr<Item> getFirstItem();
    std::shared_ptr<Item> getNextItem();
    std::shared_ptr<Item> getItemByTag(const std::string &tag);

    const std::vector<std::shared_ptr<Item>> &items() const { return _items; }

    // END Inventory

    // Effects

    void clearAllEffects();
    void applyEffect(const std::shared_ptr<Effect> &effect, DurationType durationType, float duration = 0.0f);

    std::shared_ptr<Effect> getFirstEffect();
    std::shared_ptr<Effect> getNextEffect();

    // END Effects

    // Stunt mode

    bool isStuntMode() const { return _stunt; }

    /**
     * Places this object into the stunt mode. Objects in this mode have their
     * position and orientation fixed to the world origin. Subsequent changes to
     * position and orientation will be buffered and applied when
     * stopStuntMode is called.
     */
    void startStuntMode();

    void stopStuntMode();

    // END Stunt mode

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

    virtual void clearAllActions();

    void addAction(std::shared_ptr<Action> action);
    void addActionOnTop(std::shared_ptr<Action> action);
    void delayAction(std::shared_ptr<Action> action, float seconds);

    bool hasUserActionsPending() const;

    std::shared_ptr<Action> getCurrentAction() const;

    const std::deque<std::shared_ptr<Action>> &actions() const { return _actions; }

    // END Actions

    // Local variables

    bool getLocalBoolean(int index) const;
    int getLocalNumber(int index) const;

    const std::map<int, bool> &localBooleans() const { return _localBooleans; }
    const std::map<int, int> &localNumbers() const { return _localNumbers; }

    void setLocalBoolean(int index, bool value);
    void setLocalNumber(int index, int value);

    // END Local variables

    // Scripts

    const std::string &getOnHeartbeat() const { return _onHeartbeat; }
    const std::string &getOnUserDefined() const { return _onUserDefined; }

    // END Scripts

protected:
    struct DelayedAction {
        std::shared_ptr<Action> action;
        std::unique_ptr<Timer> timer;
    };

    struct AppliedEffect {
        std::shared_ptr<Effect> effect;
        DurationType durationType {DurationType::Instant};
        float duration {0.0f};
    };

    uint32_t _id;
    ObjectType _type;
    std::string _sceneName;
    Game &_game;
    ServicesView &_services;

    std::string _tag;
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

    glm::vec3 _position {0.0f};
    glm::quat _orientation {1.0f, 0.0f, 0.0f, 0.0f};
    glm::mat4 _transform {1.0f};
    bool _visible {true};
    Room *_room {nullptr};
    std::vector<std::shared_ptr<Item>> _items;
    std::deque<AppliedEffect> _effects;
    bool _open {false};
    bool _stunt {false};
    std::string _activeAnimName;

    std::shared_ptr<scene::SceneNode> _sceneNode;

    int _itemIndex {0};
    int _effectIndex {0};

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
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        _id(id),
        _type(type),
        _sceneName(std::move(sceneName)),
        _game(game),
        _services(services) {
    }

    virtual void updateTransform();

    // Actions

    void updateActions(float dt);
    void removeCompletedActions();
    void updateDelayedActions(float dt);

    void executeActions(float dt);

    // END Actions

    // Effects

    void updateEffects(float dt);
    void applyInstantEffect(Effect &effect);

    // END Effects
};

} // namespace game

} // namespace reone
