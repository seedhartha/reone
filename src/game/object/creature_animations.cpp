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

/** @file
 *  Animation-related Creature functions.
 */

#include "creature.h"

#include <boost/format.hpp>

#include "../../common/log.h"

using namespace std;

namespace reone {

namespace game {

string Creature::getAnimationName(AnimationType anim) const {
    string result;
    switch (anim) {
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
            return getFirstIfCreatureModel("", "tlklaugh");
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
            return getFirstIfCreatureModel("", "deactivate");
        case AnimationType::LoopingSpasm:
            return getFirstIfCreatureModel("cspasm", "spasm");
        case AnimationType::LoopingSleep:
            return "sleep";
        case AnimationType::LoopingProne:
            return "prone";
        case AnimationType::LoopingPause3:
            return getFirstIfCreatureModel("", "pause3");
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
        case AnimationType::LoopingReady:
            return getAnimationName(CombatAnimation::Ready, getWieldType(), 0);

        case AnimationType::LoopingWorship:
        case AnimationType::LoopingGetLow:
        case AnimationType::LoopingGetMid:
        case AnimationType::LoopingPauseDrunk:
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
            debug("CreatureAnimationResolver: unsupported animation type: " + to_string(static_cast<int>(anim)), 2);
            return "";
    }
}

string Creature::getDieAnimation() const {
    return getFirstIfCreatureModel("cdie", "die");
}

string Creature::getFirstIfCreatureModel(string creatureAnim, string elseAnim) const {
    return _modelType == Creature::ModelType::Creature ? move(creatureAnim) : move(elseAnim);
}

string Creature::getDeadAnimation() const {
    return getFirstIfCreatureModel("cdead", "dead");
}

string Creature::getPauseAnimation() const {
    if (_modelType == Creature::ModelType::Creature) return "cpause1";

    // TODO: if (_lowHP) return "pauseinj" 

    if (_inCombat) {
        WeaponType type = WeaponType::None;
        WeaponWield wield = WeaponWield::None;
        getWeaponInfo(type, wield);

        int wieldNumber = getWeaponWieldNumber(wield);
        return str(boost::format("g%dr1") % wieldNumber);
    }

    return "pause1";
}

bool Creature::getWeaponInfo(WeaponType &type, WeaponWield &wield) const {
    shared_ptr<Item> item(getEquippedItem(InventorySlot::rightWeapon));
    if (item) {
        type = item->weaponType();
        wield = item->weaponWield();
        return true;
    }

    return false;
}

int Creature::getWeaponWieldNumber(WeaponWield wield) const {
    switch (wield) {
        case WeaponWield::StunBaton:
            return 1;
        case WeaponWield::SingleSword:
            return isSlotEquipped(InventorySlot::leftWeapon) ? 4 : 2;
        case WeaponWield::DoubleBladedSword:
            return 3;
        case WeaponWield::BlasterPistol:
            return isSlotEquipped(InventorySlot::leftWeapon) ? 6 : 5;
        case WeaponWield::BlasterRifle:
            return 7;
        case WeaponWield::HeavyWeapon:
            return 9;
        default:
            return 8;
    }
}

string Creature::getWalkAnimation() const {
    return getFirstIfCreatureModel("cwalk", "walk");
}

string Creature::getRunAnimation() const {
    if (_modelType == Creature::ModelType::Creature) return "crun";

    // TODO: if (_lowHP) return "runinj" 

    if (_inCombat) {
        WeaponType type = WeaponType::None;
        WeaponWield wield = WeaponWield::None;
        getWeaponInfo(type, wield);

        switch (wield) {
            case WeaponWield::SingleSword:
                return isSlotEquipped(InventorySlot::leftWeapon) ? "runds" : "runss";
            case WeaponWield::DoubleBladedSword:
                return "runst";
            case WeaponWield::BlasterRifle:
            case WeaponWield::HeavyWeapon:
                return "runrf";
            default:
                break;
        }
    }

    return "run";
}

string Creature::getTalkNormalAnimation() const {
    return "tlknorm";
}

string Creature::getHeadTalkAnimation() const {
    return "talk";
}

static string formatCombatAnimation(const string &format, CreatureWieldType wield, int variant) {
    return str(boost::format(format) % static_cast<int>(wield) % variant);
}

string Creature::getAnimationName(CombatAnimation anim, CreatureWieldType wield, int variant) const {
    switch (anim) {
        case CombatAnimation::Draw:
            return getFirstIfCreatureModel("", formatCombatAnimation("g%dw%d", wield, 1));
        case CombatAnimation::Ready:
            return getFirstIfCreatureModel("creadyr", formatCombatAnimation("g%dr%d", wield, 1));
        case CombatAnimation::Attack:
            return getFirstIfCreatureModel("g0a1", formatCombatAnimation("g%da%d", wield, variant));
        case CombatAnimation::Damage:
            return getFirstIfCreatureModel("cdamages", formatCombatAnimation("g%dd%d", wield, variant));
        case CombatAnimation::Dodge:
            return getFirstIfCreatureModel("cdodgeg", formatCombatAnimation("g%dg%d", wield, variant));
        case CombatAnimation::MeleeAttack:
            return getFirstIfCreatureModel("m0a1", formatCombatAnimation("m%da%d", wield, variant));
        case CombatAnimation::MeleeDamage:
            return getFirstIfCreatureModel("cdamages", formatCombatAnimation("m%dd%d", wield, variant));
        case CombatAnimation::MeleeDodge:
            return getFirstIfCreatureModel("cdodgeg", formatCombatAnimation("m%dg%d", wield, variant));
        case CombatAnimation::CinematicMeleeAttack:
            return formatCombatAnimation("c%da%d", wield, variant);
        case CombatAnimation::CinematicMeleeDamage:
            return formatCombatAnimation("c%dd%d", wield, variant);
        case CombatAnimation::CinematicMeleeParry:
            return formatCombatAnimation("c%dp%d", wield, variant);
        case CombatAnimation::BlasterAttack:
            return getFirstIfCreatureModel("b0a1", formatCombatAnimation("b%da%d", wield, variant));
        default:
            return "";
    }
}

} // namespace game

} // namespace reone
