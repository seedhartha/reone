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

#include "creatureanimresolver.h"

#include <stdexcept>

#include <boost/format.hpp>

#include "../../common/log.h"

#include "creature.h"

using namespace std;

namespace reone {

namespace game {

static string g_animDeadCharacter("dead");
static string g_animDeadCreature("cdead");
static string g_animDieCharacter("die");
static string g_animDieCreature("cdie");
static string g_animGreeting("greeting");
static string g_animPauseCharacter("pause1");
static string g_animPauseCreature("cpause1");
static string g_animRunCharacter("run");
static string g_animRunCreature("crun");
static string g_animTalk("talk");
static string g_animTalkForceful("tlkforce");
static string g_animTalkInjured("talkinj");
static string g_animTalkLaughing("tlklaugh");
static string g_animTalkNormal("tlknorm");
static string g_animTalkPleading("tlkplead");
static string g_animTalkSad("tlksad");
static string g_animUnlockDoor("unlockdr");
static string g_animVictory("victory");
static string g_animWalkCharacter("walk");
static string g_animWalkCreature("cwalk");
static string g_animWeld("weld");

CreatureAnimationResolver::CreatureAnimationResolver(const Creature *creature) : _creature(creature) {
    if (!creature) {
        throw invalid_argument("creature must not be null");
    }
}

string CreatureAnimationResolver::getAnimationName(AnimationType animation) const {
    static string empty;

    string result;
    switch (animation) {
        case AnimationType::LoopingPause:
            return getPauseAnimation();
        case AnimationType::LoopingTalkSad:
            return g_animTalkSad;
        case AnimationType::LoopingTalkNormal:
            return g_animTalkNormal;
        case AnimationType::LoopingTalkPleading:
            return g_animTalkPleading;
        case AnimationType::LoopingTalkForceful:
            return g_animTalkForceful;
        case AnimationType::LoopingTalkLaughing:
            return g_animTalkLaughing;
        case AnimationType::LoopingDead:
            return getDeadAnimation();
        case AnimationType::LoopingTalkInjured:
            return g_animTalkInjured;
        case AnimationType::LoopingUnlockDoor:
            return g_animUnlockDoor;
        case AnimationType::FireForgetVictory1:
            return g_animVictory;
        case AnimationType::LoopingWeld:
            return g_animWeld;

        case AnimationType::LoopingPause2:
        case AnimationType::LoopingListen:
        case AnimationType::LoopingMeditate:
        case AnimationType::LoopingWorship:
        case AnimationType::LoopingGetLow:
        case AnimationType::LoopingGetMid:
        case AnimationType::LoopingPauseTired:
        case AnimationType::LoopingPauseDrunk:
        case AnimationType::LoopingFlirt:
        case AnimationType::LoopingUseComputer:
        case AnimationType::LoopingDance:
        case AnimationType::LoopingDance1:
        case AnimationType::LoopingHorror:
        case AnimationType::LoopingReady:
        case AnimationType::LoopingDeactivate:
        case AnimationType::LoopingSpasm:
        case AnimationType::LoopingSleep:
        case AnimationType::LoopingProne:
        case AnimationType::LoopingPause3:
        case AnimationType::LoopingListenInjured:
        case AnimationType::LoopingTreatInjured:
        case AnimationType::LoopingDeadProne:
        case AnimationType::LoopingKneelTalkAngry:
        case AnimationType::LoopingKneelTalkSad:
        case AnimationType::LoopingCheckBody:
        case AnimationType::LoopingSitAndMeditate:
        case AnimationType::LoopingSitChair:
        case AnimationType::LoopingSitChairDrink:
        case AnimationType::LoopingSitChairPazak:
        case AnimationType::LoopingSitChairComp1:
        case AnimationType::LoopingSitChairComp2:
        case AnimationType::LoopingRage:
        case AnimationType::LoopingClosed:
        case AnimationType::LoopingStealth:
        case AnimationType::LoopingChokeWorking:
        case AnimationType::LoopingMeditateStand:
        case AnimationType::LoopingChoke:
        case AnimationType::FireForgetHeadTurnLeft:
        case AnimationType::FireForgetHeadTurnRight:
        case AnimationType::FireForgetPauseScratchHead:
        case AnimationType::FireForgetPauseBored:
        case AnimationType::FireForgetSalute:
        case AnimationType::FireForgetBow:
        case AnimationType::FireForgetGreeting:
        case AnimationType::FireForgetTaunt:
        case AnimationType::FireForgetVictory2:
        case AnimationType::FireForgetVictory3:
        case AnimationType::FireForgetInject:
        case AnimationType::FireForgetUseComputer:
        case AnimationType::FireForgetPersuade:
        case AnimationType::FireForgetActivate:
        case AnimationType::FireForgetThrowHigh:
        case AnimationType::FireForgetThrowLow:
        case AnimationType::FireForgetCustom01:
        case AnimationType::FireForgetTreatInjured:
        case AnimationType::FireForgetForceCast:
        case AnimationType::FireForgetOpen:
        case AnimationType::FireForgetDiveRoll:
        case AnimationType::FireForgetScream:
        default:
            warn("CreatureAnimationResolver: unsupported animation type: " + to_string(static_cast<int>(animation)));
            return empty;
    }
}

string CreatureAnimationResolver::getDieAnimation() const {
    return _creature->modelType() == Creature::ModelType::Creature ? g_animDieCreature : g_animDieCharacter;
}

string CreatureAnimationResolver::getDeadAnimation() const {
    return _creature->modelType() == Creature::ModelType::Creature ? g_animDeadCreature : g_animDeadCharacter;
}

string CreatureAnimationResolver::getPauseAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) {
        return g_animPauseCreature;
    }

    // TODO: if (_lowHP) return "pauseinj" 

    if (_creature->isInCombat()) {
        WeaponType type = WeaponType::None;
        WeaponWield wield = WeaponWield::None;
        getWeaponInfo(type, wield);

        int wieldNumber = getWeaponWieldNumber(wield);
        return str(boost::format("g%dr1") % wieldNumber);
    }

    return g_animPauseCharacter;
}

bool CreatureAnimationResolver::getWeaponInfo(WeaponType &type, WeaponWield &wield) const {
    shared_ptr<Item> item(_creature->getEquippedItem(kInventorySlotRightWeapon));
    if (item) {
        type = item->weaponType();
        wield = item->weaponWield();
        return true;
    }

    return false;
}

int CreatureAnimationResolver::getWeaponWieldNumber(WeaponWield wield) const {
    switch (wield) {
        case WeaponWield::StunBaton:
            return 1;
        case WeaponWield::SingleSaber:
            return _creature->isSlotEquipped(kInventorySlotLeftWeapon) ? 4 : 2;
        case WeaponWield::TwoHandedSaber:
            return 3;
        case WeaponWield::SingleBlaster:
            return _creature->isSlotEquipped(kInventorySlotLeftWeapon) ? 6 : 5;
        case WeaponWield::Rifle:
            return 7;
        case WeaponWield::HeavyCarbine:
            return 9;
        default:
            return 8;
    }
}

string CreatureAnimationResolver::getWalkAnimation() const {
    switch (_creature->modelType()) {
        case Creature::ModelType::Creature:
            return g_animWalkCreature;
        default:
            return g_animWalkCharacter;
    }
}

string CreatureAnimationResolver::getRunAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) {
        return g_animRunCreature;
    }

    // TODO: if (_lowHP) return "runinj" 

    if (_creature->isInCombat()) {
        WeaponType type = WeaponType::None;
        WeaponWield wield = WeaponWield::None;
        getWeaponInfo(type, wield);

        switch (wield) {
            case WeaponWield::SingleSaber:
                return _creature->isSlotEquipped(kInventorySlotLeftWeapon) ? "runds" : "runss";
            case WeaponWield::TwoHandedSaber:
                return "runst";
            case WeaponWield::Rifle:
            case WeaponWield::HeavyCarbine:
                return "runrf";
            default:
                break;
        }
    }

    return g_animRunCharacter;
}

string CreatureAnimationResolver::getTalkNormalAnimation() const {
    return g_animTalkNormal;
}

string CreatureAnimationResolver::getHeadTalkAnimation() const {
    return g_animTalk;
}

string CreatureAnimationResolver::getDuelAttackAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) return "g0a1";

    WeaponType type = WeaponType::None;
    WeaponWield wield = WeaponWield::None;
    getWeaponInfo(type, wield);

    int wieldNumber = getWeaponWieldNumber(wield);

    switch (type) {
        case WeaponType::Melee:
            return str(boost::format("c%da1") % wieldNumber);
        case WeaponType::Ranged:
            return str(boost::format("b%da1") % wieldNumber);
        default:
            return str(boost::format("g%da1") % wieldNumber);
    }
}

string CreatureAnimationResolver::getBashAttackAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) return "g0a2";

    WeaponType type = WeaponType::None;
    WeaponWield wield = WeaponWield::None;
    getWeaponInfo(type, wield);

    int wieldNumber = getWeaponWieldNumber(wield);

    switch (type) {
        case WeaponType::Melee:
            return str(boost::format("c%da2") % wieldNumber);
        case WeaponType::Ranged:
            return str(boost::format("b%da2") % wieldNumber);
        default:
            return str(boost::format("g%da2") % wieldNumber);
    }
}

string CreatureAnimationResolver::getDodgeAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) return "cdodgeg";

    WeaponType type = WeaponType::None;
    WeaponWield wield = WeaponWield::None;
    getWeaponInfo(type, wield);

    int wieldNumber = getWeaponWieldNumber(wield);

    return str(boost::format("g%dg1") % wieldNumber);
}

string CreatureAnimationResolver::getKnockdownAnimation() const {
    return _creature->modelType() == Creature::ModelType::Creature ? "ckdbck" : "g1y1";
}

} // namespace game

} // namespace reone
