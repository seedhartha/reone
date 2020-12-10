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

#include "creatureanimresolver.h"

#include <stdexcept>

#include <boost/format.hpp>

#include "creature.h"

using namespace std;

namespace reone {

namespace game {

static string g_animPauseCreature("cpause1");
static string g_animPauseCharacter("pause1");
static string g_animWalkCreature("cwalk");
static string g_animWalkCharacter("walk");
static string g_animRunCreature("crun");
static string g_animRunCharacter("run");
static string g_animTalkHead("talk");
static string g_animTalkBody("tlknorm");
static string g_animGreeting("greeting");
static string g_animUnlockDoor("unlockdr");

CreatureAnimationResolver::CreatureAnimationResolver(const Creature *creature) : _creature(creature) {
    if (!creature) {
        throw invalid_argument("creature must not be null");
    }
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

string CreatureAnimationResolver::getUnlockDoorAnimation() const {
    return g_animUnlockDoor;
}

string CreatureAnimationResolver::getTalkAnimation() const {
    return g_animTalkBody;
}

string CreatureAnimationResolver::getHeadTalkAnimation() const {
    return g_animTalkHead;
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
