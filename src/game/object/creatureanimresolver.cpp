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
        case AnimationType::LoopingPause2:
            return getFirstIfCreatureModel("cpause2", "pause2");
        case AnimationType::LoopingListen:
            return "listen";
        case AnimationType::LoopingMeditate:
            return "meditate";
        case AnimationType::LoopingTalkNormal:
            return "tlknorm";
        case AnimationType::LoopingTalkPleading:
            return "tlkplead";
        case AnimationType::LoopingTalkForceful:
            return "tlkforce";
        case AnimationType::LoopingTalkLaughing:
            return getFirstIfCreatureModel(empty, "tlklaugh");
        case AnimationType::LoopingTalkSad:
            return "tlksad";
        case AnimationType::LoopingPauseTired:
            return "pausetrd";
        case AnimationType::LoopingFlirt:
            return "flirt";
        case AnimationType::LoopingUseComputer:
            return "usecomplp";
        case AnimationType::LoopingDance:
            return "dance";
        case AnimationType::LoopingDance1:
            return "dance1";
        case AnimationType::LoopingHorror:
            return "horror";
        case AnimationType::LoopingDeactivate:
            return getFirstIfCreatureModel(empty, "deactivate");
        case AnimationType::LoopingSpasm:
            return getFirstIfCreatureModel("cspasm", "spasm");
        case AnimationType::LoopingSleep:
            return "sleep";
        case AnimationType::LoopingProne:
            return "prone";
        case AnimationType::LoopingPause3:
            return getFirstIfCreatureModel(empty, "pause3");
        case AnimationType::LoopingWeld:
            return "weld";
        case AnimationType::LoopingDead:
            return getDeadAnimation();
        case AnimationType::LoopingTalkInjured:
            return "talkinj";
        case AnimationType::LoopingListenInjured:
            return "listeninj";
        case AnimationType::LoopingTreatInjured:
            return "treatinjlp";
        case AnimationType::LoopingUnlockDoor:
            return "unlockdr";
        case AnimationType::LoopingClosed:
            return "closed";
        case AnimationType::LoopingStealth:
            return "stealth";
        case AnimationType::FireForgetHeadTurnLeft:
            return getFirstIfCreatureModel("chturnl", "hturnl");
        case AnimationType::FireForgetHeadTurnRight:
            return getFirstIfCreatureModel("chturnr", "hturnr");
        case AnimationType::FireForgetSalute:
            return "salute";
        case AnimationType::FireForgetBow:
            return "bow";
        case AnimationType::FireForgetGreeting:
            return "greeting";
        case AnimationType::FireForgetTaunt:
            return getFirstIfCreatureModel("ctaunt", "taunt");
        case AnimationType::FireForgetVictory1:
            return getFirstIfCreatureModel("cvictory", "victory");
        case AnimationType::FireForgetInject:
            return "inject";
        case AnimationType::FireForgetUseComputer:
            return "usecomp";
        case AnimationType::FireForgetPersuade:
            return "persuade";
        case AnimationType::FireForgetActivate:
            return "activate";
        case AnimationType::LoopingChoke:
            return "choke";
        case AnimationType::FireForgetTreatInjured:
            return "treatinj";
        case AnimationType::FireForgetOpen:
            return "open";

        case AnimationType::LoopingWorship:
        case AnimationType::LoopingGetLow:
        case AnimationType::LoopingGetMid:
        case AnimationType::LoopingPauseDrunk:
        case AnimationType::LoopingReady:
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
        case AnimationType::LoopingChokeWorking:
        case AnimationType::LoopingMeditateStand:
        case AnimationType::FireForgetPauseScratchHead:
        case AnimationType::FireForgetPauseBored:
        case AnimationType::FireForgetVictory2:
        case AnimationType::FireForgetVictory3:
        case AnimationType::FireForgetThrowHigh:
        case AnimationType::FireForgetThrowLow:
        case AnimationType::FireForgetCustom01:
        case AnimationType::FireForgetForceCast:
        case AnimationType::FireForgetDiveRoll:
        case AnimationType::FireForgetScream:
        default:
            debug("CreatureAnimationResolver: unsupported animation type: " + to_string(static_cast<int>(animation)), 2);
            return empty;
    }
}

string CreatureAnimationResolver::getDieAnimation() const {
    return getFirstIfCreatureModel("cdie", "die");
}

string CreatureAnimationResolver::getFirstIfCreatureModel(string creatureAnim, string elseAnim) const {
    return _creature->modelType() == Creature::ModelType::Creature ? move(creatureAnim) : move(elseAnim);
}

string CreatureAnimationResolver::getDeadAnimation() const {
    return getFirstIfCreatureModel("cdead", "dead");
}

string CreatureAnimationResolver::getPauseAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) return "cpause1";

    // TODO: if (_lowHP) return "pauseinj" 

    if (_creature->isInCombat()) {
        WeaponType type = WeaponType::None;
        WeaponWield wield = WeaponWield::None;
        getWeaponInfo(type, wield);

        int wieldNumber = getWeaponWieldNumber(wield);
        return str(boost::format("g%dr1") % wieldNumber);
    }

    return "pause1";
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
    return getFirstIfCreatureModel("cwalk", "walk");
}

string CreatureAnimationResolver::getRunAnimation() const {
    if (_creature->modelType() == Creature::ModelType::Creature) return "crun";

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

    return "run";
}

string CreatureAnimationResolver::getTalkNormalAnimation() const {
    return "tlknorm";
}

string CreatureAnimationResolver::getHeadTalkAnimation() const {
    return "talk";
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
    return getFirstIfCreatureModel("ckdbck", "g1y1");
}

} // namespace game

} // namespace reone
