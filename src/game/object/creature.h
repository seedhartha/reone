/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../resource/2dafile.h"
#include "../../resource/gfffile.h"
#include "../../script/types.h"

#include "../rp/attributes.h"

#include "creatureanimresolver.h"
#include "creaturemodelbuilder.h"
#include "item.h"
#include "spatial.h"

namespace reone {

class Timer;

namespace game {

constexpr float kDefaultAttackRange = 2.0f;

class CreatureBlueprint;
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

    Creature(uint32_t id, ObjectFactory *objectFactory, scene::SceneGraph *sceneGraph);

    void update(float dt) override;
    void clearAllActions() override;

    bool isSelectable() const override;

    glm::vec3 getSelectablePosition() const override;

    void load(const resource::GffStruct &gffs);
    void load(const std::shared_ptr<CreatureBlueprint> &blueprint);
    void load(const CreatureConfiguration &config);

    void giveXP(int amount);

    bool isMovementRestricted() const;
    bool isInCombat() const;

    float getAttackRange() const;

    Gender gender() const;
    ModelType modelType() const;
    int appearance() const;
    std::shared_ptr<render::Texture> portrait() const;
    float walkSpeed() const;
    float runSpeed() const;
    CreatureAttributes &attributes();
    Faction faction() const;
    int xp() const;

    void setMovementType(MovementType type);
    void setTalking(bool talking);
    void setFaction(Faction faction);
    void setMovementRestricted(bool restricted);
    void setInCombat(bool inCombat);
    void setImmortal(bool immortal);
    void setXP(int xp);

    // Animation

    // END Animation

    void playAnimation(Animation animation, float speed = 1.0f) override;

    void playAnimation(CombatAnimation animation);
    void playAnimation(const std::string &name, bool looping = false, float speed = 1.0f);

    void updateModelAnimation();

    // Equipment

    void equip(const std::string &resRef);
    void equip(InventorySlot slot, const std::shared_ptr<Item> &item);
    void unequip(const std::shared_ptr<Item> &item);

    bool isSlotEquipped(InventorySlot slot) const;

    std::shared_ptr<Item> getEquippedItem(InventorySlot slot) const;

    const std::map<InventorySlot, std::shared_ptr<Item>> &equipment() const;

    // END Equipment

    // Pathfinding

    void setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound);
    void clearPath();

    std::shared_ptr<Path> &path();

    // END Pathfinding

    // Scripts

    void runSpawnScript();

    // END Scripts

private:
    CreatureConfiguration _config;
    int _appearance { 0 };
    ModelType _modelType { ModelType::Creature };
    std::shared_ptr<scene::ModelSceneNode> _headModel;
    std::shared_ptr<render::Texture> _portrait;
    std::map<InventorySlot, std::shared_ptr<Item>> _equipment;
    std::shared_ptr<Path> _path;
    float _walkSpeed { 0.0f };
    float _runSpeed { 0.0f };
    MovementType _movementType { MovementType::None };
    bool _talking { false };
    CreatureAttributes _attributes;
    bool _animDirty { true };
    bool _animFireForget { false };
    Faction _faction { Faction::Invalid };
    bool _movementRestricted { false };
    bool _inCombat { false };
    int _portraitId { 0 };
    CreatureAnimationResolver _animResolver;
    CreatureModelBuilder _modelBuilder;
    bool _immortal { false };
    int _xp { 0 };

    // Scripts

    std::string _onSpawn;

    // END Scripts

    void loadTransform(const resource::GffStruct &gffs);
    void loadBlueprint(const resource::GffStruct &gffs);
    void loadAppearance(const resource::TwoDaTable &table, int row);
    void loadPortrait(int appearance);

    void updateModel();
    void updateHealth();

    ModelType parseModelType(const std::string &s) const;

    friend class CreatureBlueprint;
};

} // namespace game

} // namespace reone
