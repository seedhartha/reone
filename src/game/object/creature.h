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
#include <queue>

#include "../../resource/2dafile.h"
#include "../../resource/gfffile.h"
#include "../../script/types.h"

#include "../enginetype/effect.h"
#include "../rp/attributes.h"

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
    enum class MovementType {
        None,
        Walk,
        Run
    };

    enum class Animation {
        UnlockDoor,
        DuelAttack,
        BashAttack,
        Dodge,
        Knockdown
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

    glm::vec3 selectablePosition() const override;

    void load(const resource::GffStruct &gffs);
    void load(const std::shared_ptr<CreatureBlueprint> &blueprint);
    void load(const CreatureConfiguration &config);

    void playAnimation(Animation anim);
    void updateModelAnimation();

    void applyEffect(std::unique_ptr<Effect> &&eff);

    bool isMovementRestricted() const;

    Gender gender() const;
    int appearance() const;
    std::shared_ptr<render::Texture> portrait() const;
    float walkSpeed() const;
    float runSpeed() const;
    const CreatureAttributes &attributes() const;
    std::shared_ptr<CreatureBlueprint> blueprint() const;
    Faction faction() const;
    int attackRange() const;

    void setTag(const std::string &tag);
    void setMovementType(MovementType type);
    void setTalking(bool talking);
    void setFaction(Faction faction);
    void setMovementRestricted(bool restricted);

    // Equipment

    void equip(const std::string &resRef);
    void equip(InventorySlot slot, const std::shared_ptr<Item> &item);
    void unequip(const std::shared_ptr<Item> &item);

    std::shared_ptr<Item> getEquippedItem(InventorySlot slot) const;

    const std::map<InventorySlot, std::shared_ptr<Item>> &equipment() const;

    // END Equipment

    // Pathfinding

    std::shared_ptr<Path> &path();

    void setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound);
    void clearPath();

    // END Pathfinding

private:
    enum class ModelType {
        Creature,
        Droid,
        Character
    };

    ObjectFactory *_objectFactory { nullptr };
    CreatureConfiguration _config;
    std::shared_ptr<CreatureBlueprint> _blueprint;
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
    std::deque<std::unique_ptr<Effect>> _effects;
    bool _movementRestricted { false };

    // Scripts

    std::string _onSpawn;

    // END Scripts

    void loadAppearance(const resource::TwoDaTable &table, int row);
    void loadPortrait(int appearance);
    void updateModel();

    ModelType parseModelType(const std::string &s) const;
    bool getWeaponInfo(WeaponType &type, WeaponWield &wield) const;
    int getWeaponWieldNumber(WeaponWield wield) const;

    std::string getBodyModelName() const;
    std::string getBodyTextureName() const;
    std::string getHeadModelName() const;
    std::string getWeaponModelName(InventorySlot slot) const;

    const std::string &getPauseAnimation() const;
    const std::string &getRunAnimation() const;
    const std::string &getWalkAnimation() const;

    std::string getDuelAttackAnimation() const;
    std::string getBashAttackAnimation() const;
    std::string getDodgeAnimation() const;
    std::string getKnockdownAnimation() const;
};

} // namespace game

} // namespace reone
