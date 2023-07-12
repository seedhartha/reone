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

#include "reone/audio/buffer.h"
#include "reone/audio/source.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/types.h"
#include "reone/scene/animeventlistener.h"
#include "reone/scene/node/model.h"
#include "reone/script/types.h"
#include "reone/system/timer.h"

#include "../d20/attributes.h"
#include "../object.h"
#include "../schema/git.h"
#include "../schema/utc.h"

#include "item.h"

namespace reone {

namespace game {

constexpr float kDefaultAttackRange = 2.0f;

class Creature : public Object, public scene::IAnimationEventListener {
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
        glm::vec3 destination {0.0f};
        std::vector<glm::vec3> points;
        uint32_t timeFound {0};
        int pointIdx {0};

        void selectNextPoint();
    };

    struct BodyBag {
        std::string name;
        int appearance {0}; /**< index into placeables.2da */
        bool corpse {false};
    };

    struct Perception {
        float sightRange {0.0f};
        float hearingRange {0.0f};
        std::set<std::shared_ptr<Object>> seen;
        std::set<std::shared_ptr<Object>> heard;
        PerceptionType lastPerception {PerceptionType::Seen};
        std::shared_ptr<Object> lastPerceived;
    };

    struct CombatState {
        bool active {false};
        bool debilitated {false};
        std::shared_ptr<Object> attackTarget;
        Timer deactivationTimer;
    };

    Creature(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Creature,
            std::move(sceneName),
            game,
            services) {
    }

    void loadFromGIT(const schema::GIT_Creature_List &git);
    void loadFromBlueprint(const std::string &resRef);
    void loadAppearance();

    void update(float dt) override;
    void clearAllActions() override;
    void die() override;

    void giveXP(int amount);

    void playSound(SoundSetEntry entry, bool positional = true);

    void startTalking(const std::shared_ptr<graphics::LipAnimation> &animation);
    void stopTalking();

    bool isSelectable() const override;
    bool isMovementRestricted() const { return _movementRestricted; }
    bool isLevelUpPending() const;

    glm::vec3 getSelectablePosition() const override;
    float getAttackRange() const;
    int getNeededXP() const;

    Gender gender() const { return _gender; }
    ModelType modelType() const { return _modelType; }
    int appearance() const { return _appearance; }
    std::shared_ptr<graphics::Texture> portrait() const { return _portrait; }
    float walkSpeed() const { return _walkSpeed; }
    float runSpeed() const { return _runSpeed; }
    CreatureAttributes &attributes() { return _attributes; }
    Faction faction() const { return _faction; }
    int xp() const { return _xp; }
    RacialType racialType() const { return _race; }
    Subrace subrace() const { return _subrace; }
    NPCAIStyle aiStyle() const { return _aiStyle; }
    int walkmeshMaterial() const { return _walkmeshMaterial; }

    void setGender(Gender gender) { _gender = gender; }
    void setAppearance(int appearance) { _appearance = appearance; }
    void setMovementType(MovementType type);
    void setFaction(Faction faction) { _faction = faction; }
    void setMovementRestricted(bool restricted) { _movementRestricted = restricted; }
    void setImmortal(bool immortal) { _immortal = immortal; }
    void setXP(int xp) { _xp = xp; }
    void setAIStyle(NPCAIStyle style) { _aiStyle = style; }
    void setWalkmeshMaterial(int material) { _walkmeshMaterial = material; }

    // Animation

    void playAnimation(AnimationType type, scene::AnimationProperties properties = scene::AnimationProperties()) override;

    void playAnimation(CombatAnimation anim, CreatureWieldType wield, int variant = 1);
    void playAnimation(const std::string &name, scene::AnimationProperties properties = scene::AnimationProperties());
    void playAnimation(const std::shared_ptr<graphics::Animation> &anim, scene::AnimationProperties properties = scene::AnimationProperties());

    void updateModelAnimation();

    // END Animation

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

    bool navigateTo(const glm::vec3 &dest, bool run, float distance, float dt);
    void advanceOnPath(bool run, float dt);
    void updatePath(const glm::vec3 &dest);

    void clearPath();
    void setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound);

    std::shared_ptr<Path> &path() { return _path; }

    // END Pathfinding

    // Perception

    void onObjectSeen(const std::shared_ptr<Object> &object);
    void onObjectVanished(const std::shared_ptr<Object> &object);
    void onObjectHeard(const std::shared_ptr<Object> &object);
    void onObjectInaudible(const std::shared_ptr<Object> &object);

    const Perception &perception() const { return _perception; }

    // END Perception

    // Combat

    void activateCombat();
    void deactivateCombat(float delay);

    bool isInCombat() const { return _combatState.active; }
    bool isDebilitated() const { return _combatState.debilitated; }
    bool isTwoWeaponFighting() const;

    std::shared_ptr<Object> getAttemptedAttackTarget() const;
    std::shared_ptr<Object> getAttackTarget() const { return _combatState.attackTarget; }
    int getAttackBonus(bool offHand = false) const;
    int getDefense() const;
    void getMainHandDamage(int &min, int &max) const;
    void getOffhandDamage(int &min, int &max) const;

    void setAttackTarget(std::shared_ptr<Object> target) {
        _combatState.attackTarget = std::move(target);
    }

    // END Combat

    // Gold

    void giveGold(int amount);
    void takeGold(int amount);

    int gold() const { return _gold; }

    // END Gold

    // Scripts

    void runSpawnScript();
    void runEndRoundScript();

    // END Scripts

    // IAnimationEventListener

    void onEventSignalled(const std::string &name) override;

    // END IAnimationEventListener

private:
    Gender _gender {Gender::Male};
    int _appearance {0};
    ModelType _modelType {ModelType::Creature};
    std::shared_ptr<graphics::Texture> _portrait;
    std::map<int, std::shared_ptr<Item>> _equipment;
    std::shared_ptr<Path> _path;
    float _walkSpeed {0.0f};
    float _runSpeed {0.0f};
    MovementType _movementType {MovementType::None};
    bool _talking {false};
    CreatureAttributes _attributes;
    Faction _faction {Faction::Invalid};
    bool _movementRestricted {false};
    CombatState _combatState;
    int _portraitId {0};
    bool _immortal {false};
    int _xp {0};
    std::shared_ptr<SoundSet> _soundSet;
    BodyBag _bodyBag;
    Perception _perception;
    RacialType _race {RacialType::Unknown};
    Subrace _subrace {Subrace::None};
    NPCAIStyle _aiStyle {NPCAIStyle::DefaultAttack};
    bool _isPC {false};
    bool _noPermDeath {false};
    bool _notReorienting {false};
    int _bodyVariation {0};
    int _textureVar {0};
    bool _partyInteract {false};
    int _walkRate {0};
    int _naturalAC {0};
    int _forcePoints {0};
    int _currentForce {0};
    int _refBonus {0};
    int _willBonus {0};
    int _fortBonus {0};
    int _goodEvil {0};
    int _lawfulChaotic {0};
    int _challengeRating {0};
    bool _disarmable {false};
    uint32_t _footstepType {0};
    int _walkmeshMaterial {-1};
    int _gold {0}; /**< aka credits */
    std::string _envmap;

    std::shared_ptr<audio::AudioSource> _audioSourceVoice;
    std::shared_ptr<audio::AudioSource> _audioSourceFootstep;

    // Animation

    bool _animDirty {true};
    bool _animFireForget {false};
    std::shared_ptr<graphics::LipAnimation> _lipAnimation;

    // END Animation

    // Scripts

    std::string _onSpawn;
    std::string _onDeath;
    std::string _onNotice;
    std::string _onEndRound;
    std::string _onSpellAt;
    std::string _onAttacked;
    std::string _onDamaged;
    std::string _onDisturbed;
    std::string _onEndDialogue;
    std::string _onBlocked;
    std::string _onDialogue;

    // END Scripts

    void loadTransformFromGIT(const schema::GIT_Creature_List &git);

    void updateModel();
    void updateHealth();
    void updateCombat(float dt);

    inline void runDeathScript();
    inline void runOnNoticeScript();

    ModelType parseModelType(const std::string &s) const;

    // Appearance

    std::shared_ptr<scene::ModelSceneNode> buildModel();
    void finalizeModel(scene::ModelSceneNode &body);

    std::string getBodyModelName() const;
    std::string getBodyTextureName() const;
    std::string getHeadModelName() const;
    std::string getMaskModelName() const;
    std::string getWeaponModelName(int slot) const;

    // END Appearance

    // Animation

    void doPlayAnimation(bool fireForget, const std::function<void()> &callback);

    std::string getAnimationName(AnimationType anim) const override;
    std::string getAnimationName(CombatAnimation anim, CreatureWieldType wield, int variant) const;
    std::string getActiveAnimationName() const override;

    std::string getDeadAnimation() const;
    std::string getDieAnimation() const;
    std::string getHeadTalkAnimation() const;
    std::string getPauseAnimation() const;
    std::string getRunAnimation() const;
    std::string getTalkNormalAnimation() const;
    std::string getWalkAnimation() const;

    /**
     * @return creatureAnim if model type is creature, elseAnim otherwise
     */
    inline std::string getFirstIfCreatureModel(std::string creatureAnim, std::string elseAnim) const;

    bool getWeaponInfo(WeaponType &type, WeaponWield &wield) const;
    int getWeaponWieldNumber(WeaponWield wield) const;
    void getWeaponDamage(int slot, int &min, int &max) const;

    // END Animation

    // Blueprint

    void loadUTC(const schema::UTC &utc);

    void loadNameFromUTC(const schema::UTC &utc);
    void loadSoundSetFromUTC(const schema::UTC &utc);
    void loadBodyBagFromUTC(const schema::UTC &utc);
    void loadAttributesFromUTC(const schema::UTC &utc);
    void loadPerceptionRangeFromUTC(const schema::UTC &utc);

    // END Blueprint
};

} // namespace game

} // namespace reone
