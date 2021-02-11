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

#include <atomic>
#include <functional>

#include "../../audio/stream.h"
#include "../../render/lip/lipanimation.h"
#include "../../resource/format/2dafile.h"
#include "../../resource/format/gfffile.h"
#include "../../resource/types.h"
#include "../../script/types.h"

#include "../blueprint/blueprint.h"
#include "../rp/attributes.h"

#include "creatureanimresolver.h"
#include "creaturemodelbuilder.h"
#include "item.h"
#include "spatial.h"

namespace reone {

class Timer;

namespace game {

static constexpr float kDefaultAttackRange = 2.0f;

class ObjectFactory;

enum class CombatState {
    Idle,
    Attack,
    Defense,
    Cooldown,
    Staggered
};

class Creature : public SpatialObject {
public:
    enum class ModelType {
        Creature,
        Droid,
        Character
    };

    enum class MovementType {
        None,
        Walk,
        Run
    };

    struct Path {
        glm::vec3 destination { 0.0f };
        std::vector<glm::vec3> points;
        uint32_t timeFound { 0 };
        int pointIdx { 0 };
    };

    Creature(
        uint32_t id,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        ScriptRunner *scriptRunner);

    void update(float dt) override;
    void clearAllActions() override;
    void die() override;

    void load(const resource::GffStruct &gffs);
    void load(const std::shared_ptr<Blueprint<Creature>> &blueprint);

    void giveXP(int amount);

    void playSound(SoundSetEntry entry, bool positional = true);

    void startTalking(const std::shared_ptr<render::LipAnimation> &animation);
    void stopTalking();

    bool isSelectable() const override;
    bool isMovementRestricted() const { return _movementRestricted; }
    bool isInCombat() const { return _inCombat; }
    bool isLevelUpPending() const;

    glm::vec3 getSelectablePosition() const override;
    float getAttackRange() const;
    int getNeededXP() const;

    Gender gender() const { return _gender; }
    ModelType modelType() const { return _modelType; }
    int appearance() const { return _appearance; }
    std::shared_ptr<render::Texture> portrait() const { return _portrait; }
    float walkSpeed() const { return _walkSpeed; }
    float runSpeed() const { return _runSpeed; }
    CreatureAttributes &attributes() { return _attributes; }
    Faction faction() const { return _faction; }
    int xp() const { return _xp; }

    void setMovementType(MovementType type);
    void setFaction(Faction faction);
    void setMovementRestricted(bool restricted);
    void setInCombat(bool inCombat);
    void setImmortal(bool immortal);
    void setXP(int xp);

    // Animation

    // END Animation

    void playAnimation(AnimationType type, scene::AnimationProperties properties = scene::AnimationProperties(), std::shared_ptr<Action> actionToComplete = nullptr) override;

    void playAnimation(CombatAnimation anim, CreatureWieldType wield, int variant = 1);
    void playAnimation(const std::string &name, scene::AnimationProperties properties = scene::AnimationProperties(), std::shared_ptr<Action> actionToComplete = nullptr);
    void playAnimation(const std::shared_ptr<render::Animation> &anim, scene::AnimationProperties properties = scene::AnimationProperties());

    void updateModelAnimation();

    // Equipment

    bool equip(const std::string &resRef);
    bool equip(int slot, const std::shared_ptr<Item> &item);
    void unequip(const std::shared_ptr<Item> &item);

    bool isSlotEquipped(int slot) const;

    std::shared_ptr<Item> getEquippedItem(int slot) const;
    CreatureWieldType getWieldType() const;

    const std::map<int, std::shared_ptr<Item>> &equipment() const { return _equipment; }

    // END Equipment

    // Pathfinding

    void setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound);
    void clearPath();

    std::shared_ptr<Path> &path() { return _path; }

    // END Pathfinding

    // Scripts

    void runSpawnScript();

    // END Scripts

private:
    Gender _gender { Gender::Male };
    int _appearance { 0 };
    ModelType _modelType { ModelType::Creature };
    std::shared_ptr<scene::ModelSceneNode> _headModel;
    std::shared_ptr<render::Texture> _portrait;
    std::map<int, std::shared_ptr<Item>> _equipment;
    std::shared_ptr<Path> _path;
    float _walkSpeed { 0.0f };
    float _runSpeed { 0.0f };
    MovementType _movementType { MovementType::None };
    bool _talking { false };
    CreatureAttributes _attributes;
    Faction _faction { Faction::Invalid };
    bool _movementRestricted { false };
    bool _inCombat { false };
    int _portraitId { 0 };
    CreatureModelBuilder _modelBuilder;
    bool _immortal { false };
    int _xp { 0 };
    std::shared_ptr<SoundSet> _soundSet;

    // Animation

    CreatureAnimationResolver _animResolver;
    bool _animDirty { true };
    bool _animFireForget { false };
    std::shared_ptr<Action> _animAction; /**< action to complete when animation is finished */
    std::shared_ptr<render::LipAnimation> _lipAnimation;

    // END Animation

    // Scripts

    std::string _onSpawn;
    std::string _onDeath;

    // END Scripts

    void updateModel();
    void updateHealth();

    void runDeathScript();

    ModelType parseModelType(const std::string &s) const;

    // Loading

    void loadTransform(const resource::GffStruct &gffs);
    void loadBlueprint(const resource::GffStruct &gffs);
    void loadAppearance(const resource::TwoDaTable &table, int row);
    void loadPortrait(int appearance);

    // END Loading

    // Animation

    void doPlayAnimation(bool fireForget, const std::function<void()> &callback);

    // END Animation

    friend class CreatureBlueprint;
    friend class StaticCreatureBlueprint;
};

} // namespace game

} // namespace reone
