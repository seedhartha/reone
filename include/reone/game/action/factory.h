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

#include "attackobject.h"
#include "barkstring.h"
#include "castfakespellatlocation.h"
#include "castfakespellatobject.h"
#include "castspellatlocation.h"
#include "castspellatobject.h"
#include "closedoor.h"
#include "docommand.h"
#include "equipitem.h"
#include "equipmostdamagingmelee.h"
#include "equipmostdamagingranged.h"
#include "equipmosteffectivearmor.h"
#include "follow.h"
#include "followleader.h"
#include "followowner.h"
#include "giveitem.h"
#include "interactobject.h"
#include "jumptolocation.h"
#include "jumptoobject.h"
#include "lockobject.h"
#include "moveawayfromlocation.h"
#include "moveawayfromobject.h"
#include "movetolocation.h"
#include "movetoobject.h"
#include "movetopoint.h"
#include "opencontainer.h"
#include "opendoor.h"
#include "openlock.h"
#include "pauseconversation.h"
#include "pickupitem.h"
#include "playanimation.h"
#include "putdownitem.h"
#include "randomwalk.h"
#include "resumeconversation.h"
#include "speakstring.h"
#include "speakstringbystrref.h"
#include "startconversation.h"
#include "surrendertoenemies.h"
#include "switchweapons.h"
#include "takeitem.h"
#include "unequipitem.h"
#include "unlockobject.h"
#include "usefeat.h"
#include "useskill.h"
#include "usetalentatlocation.h"
#include "usetalentonobject.h"
#include "wait.h"

namespace reone {

namespace game {

class Game;

struct ServicesView;

class ActionFactory {
public:
    ActionFactory(Game &game, ServicesView &services) :
        _game(game),
        _services(services) {
    }

    std::unique_ptr<AttackObjectAction> newAttackObject(std::shared_ptr<Object> attackee, bool passive = false) {
        return std::make_unique<AttackObjectAction>(_game, _services, std::move(attackee), passive);
    }

    std::unique_ptr<BarkStringAction> newBarkString(int strRef) {
        return std::make_unique<BarkStringAction>(_game, _services, strRef);
    }

    std::unique_ptr<CastFakeSpellAtLocationAction> newCastFakeSpellAtLocation(SpellType spell, std::shared_ptr<Location> target, ProjectilePathType projectilePathType) {
        return std::make_unique<CastFakeSpellAtLocationAction>(_game, _services, spell, std::move(target), projectilePathType);
    }

    std::unique_ptr<CastFakeSpellAtObjectAction> newCastFakeSpellAtObject(SpellType spell, std::shared_ptr<Object> target, ProjectilePathType projectilePathType) {
        return std::make_unique<CastFakeSpellAtObjectAction>(_game, _services, spell, std::move(target), projectilePathType);
    }

    std::unique_ptr<CastSpellAtLocationAction> newCastSpellAtLocation(SpellType spell, std::shared_ptr<Location> targetLocation, int metaMagic, bool cheat, ProjectilePathType projectilePathType, bool instantSpell) {
        return std::make_unique<CastSpellAtLocationAction>(_game, _services, spell, std::move(targetLocation), metaMagic, cheat, projectilePathType, instantSpell);
    }

    std::unique_ptr<CastSpellAtObjectAction> newCastSpellAtObject(SpellType spell, std::shared_ptr<Object> target, int metaMagic, bool cheat, int domainLevel, ProjectilePathType projectilePathType, bool instantSpell) {
        return std::make_unique<CastSpellAtObjectAction>(_game, _services, spell, std::move(target), metaMagic, cheat, domainLevel, projectilePathType, instantSpell);
    }

    std::unique_ptr<CloseDoorAction> newCloseDoor(std::shared_ptr<Object> door) {
        return std::make_unique<CloseDoorAction>(_game, _services, std::move(door));
    }

    std::unique_ptr<CommandAction> newDoCommand(std::shared_ptr<script::ExecutionContext> actionToDo) {
        return std::make_unique<CommandAction>(_game, _services, std::move(actionToDo));
    }

    std::unique_ptr<EquipItemAction> newEquipItem(std::shared_ptr<Item> item, int inventorySlot, bool instant) {
        return std::make_unique<EquipItemAction>(_game, _services, std::move(item), inventorySlot, instant);
    }

    std::unique_ptr<EquipMostDamagingMeleeAction> newEquipMostDamagingMelee(std::shared_ptr<Object> versus, bool offHand) {
        return std::make_unique<EquipMostDamagingMeleeAction>(_game, _services, std::move(versus), offHand);
    }

    std::unique_ptr<EquipMostDamagingRangedAction> newEquipMostDamagingRanged(std::shared_ptr<Object> versus) {
        return std::make_unique<EquipMostDamagingRangedAction>(_game, _services, std::move(versus));
    }

    std::unique_ptr<EquipMostEffectiveArmorAction> newEquipMostEffectiveArmor() {
        return std::make_unique<EquipMostEffectiveArmorAction>(_game, _services);
    }

    std::unique_ptr<FollowAction> newFollow(std::shared_ptr<Object> follow, float followDistance) {
        return std::make_unique<FollowAction>(_game, _services, std::move(follow), followDistance);
    }

    std::unique_ptr<FollowLeaderAction> newFollowLeader() {
        return std::make_unique<FollowLeaderAction>(_game, _services);
    }

    std::unique_ptr<FollowOwnerAction> newFollowOwner(float range) {
        return std::make_unique<FollowOwnerAction>(_game, _services, range);
    }

    std::unique_ptr<GiveItemAction> newGiveItem(std::shared_ptr<Item> item, std::shared_ptr<Object> giveTo) {
        return std::make_unique<GiveItemAction>(_game, _services, std::move(item), std::move(giveTo));
    }

    std::unique_ptr<InteractObjectAction> newInteractObject(std::shared_ptr<Placeable> placeable) {
        return std::make_unique<InteractObjectAction>(_game, _services, std::move(placeable));
    }

    std::unique_ptr<JumpToLocationAction> newJumpToLocation(std::shared_ptr<Location> location) {
        return std::make_unique<JumpToLocationAction>(_game, _services, std::move(location));
    }

    std::unique_ptr<JumpToObjectAction> newJumpToObject(std::shared_ptr<Object> toJumpTo, bool walkStraightLine) {
        return std::make_unique<JumpToObjectAction>(_game, _services, std::move(toJumpTo), walkStraightLine);
    }

    std::unique_ptr<LockObjectAction> newLockObject(std::shared_ptr<Object> target) {
        return std::make_unique<LockObjectAction>(_game, _services, std::move(target));
    }

    std::unique_ptr<MoveAwayFromLocation> newMoveAwayFromLocation(std::shared_ptr<Location> moveAwayFrom, bool run, float moveAwayRange) {
        return std::make_unique<MoveAwayFromLocation>(_game, _services, std::move(moveAwayFrom), run, moveAwayRange);
    }

    std::unique_ptr<MoveAwayFromObject> newMoveAwayFromObject(std::shared_ptr<Object> fleeFrom, bool run, float moveAwayRange) {
        return std::make_unique<MoveAwayFromObject>(_game, _services, std::move(fleeFrom), run, moveAwayRange);
    }

    std::unique_ptr<MoveToLocationAction> newMoveToLocation(std::shared_ptr<Location> destination, bool run, bool force = false, float timeout = -1.0f) {
        return std::make_unique<MoveToLocationAction>(_game, _services, std::move(destination), run, force, timeout);
    }

    std::unique_ptr<MoveToObjectAction> newMoveToObject(std::shared_ptr<Object> moveTo, bool run, float range, bool force = false, float timeout = -1.0f) {
        return std::make_unique<MoveToObjectAction>(_game, _services, std::move(moveTo), run, range, force, timeout);
    }

    std::unique_ptr<MoveToPointAction> newMoveToPoint(glm::vec3 point) {
        return std::make_unique<MoveToPointAction>(_game, _services, std::move(point));
    }

    std::unique_ptr<OpenContainerAction> newOpenContainer(std::shared_ptr<Object> object) {
        return std::make_unique<OpenContainerAction>(_game, _services, std::move(object));
    }

    std::unique_ptr<OpenDoorAction> newOpenDoor(std::shared_ptr<Object> door) {
        return std::make_unique<OpenDoorAction>(_game, _services, std::move(door));
    }

    std::unique_ptr<OpenLockAction> newOpenLock(std::shared_ptr<Object> object) {
        return std::make_unique<OpenLockAction>(_game, _services, std::move(object));
    }

    std::unique_ptr<PauseConversationAction> newPauseConversation() {
        return std::make_unique<PauseConversationAction>(_game, _services);
    }

    std::unique_ptr<PickUpItemAction> newPickUpItem(std::shared_ptr<Item> item) {
        return std::make_unique<PickUpItemAction>(_game, _services, std::move(item));
    }

    std::unique_ptr<PlayAnimationAction> newPlayAnimation(AnimationType animation, float speed, float durationSeconds) {
        return std::make_unique<PlayAnimationAction>(_game, _services, animation, speed, durationSeconds);
    }

    std::unique_ptr<PutDownItemAction> newPutDownItem(std::shared_ptr<Item> item) {
        return std::make_unique<PutDownItemAction>(_game, _services, std::move(item));
    }

    std::unique_ptr<RandomWalkAction> newRandomWalk() {
        return std::make_unique<RandomWalkAction>(_game, _services);
    }

    std::unique_ptr<ResumeConversationAction> newResumeConversation() {
        return std::make_unique<ResumeConversationAction>(_game, _services);
    }

    std::unique_ptr<SpeakStringAction> newSpeakString(std::string stringToSpeak, int talkVolume) {
        return std::make_unique<SpeakStringAction>(_game, _services, stringToSpeak, talkVolume);
    }

    std::unique_ptr<SpeakStringByStrRefAction> newSpeakStringByStrRef(int strRef, int talkVolume) {
        return std::make_unique<SpeakStringByStrRefAction>(_game, _services, strRef, talkVolume);
    }

    std::unique_ptr<StartConversationAction> newStartConversation(std::shared_ptr<Object> objectToConverse,
                                                                  std::string dialogResRef,
                                                                  bool privateConversation = false,
                                                                  ConversationType conversationType = ConversationType::Cinematic,
                                                                  bool ignoreStartRange = false,
                                                                  std::vector<std::string> namesToIgnore = {},
                                                                  bool useLeader = false,
                                                                  int barkX = -1,
                                                                  int barkY = -1,
                                                                  bool dontClearAllActions = false) {
        return std::make_unique<StartConversationAction>(
            _game,
            _services,
            std::move(objectToConverse),
            std::move(dialogResRef),
            privateConversation,
            conversationType,
            ignoreStartRange,
            std::move(namesToIgnore),
            useLeader,
            barkX,
            barkY,
            dontClearAllActions);
    }

    std::unique_ptr<SurrenderToEnemiesAction> newSurrenderToEnemies() {
        return std::make_unique<SurrenderToEnemiesAction>(_game, _services);
    }

    std::unique_ptr<SwitchWeaponsAction> newSwitchWeapons() {
        return std::make_unique<SwitchWeaponsAction>(_game, _services);
    }

    std::unique_ptr<TakeItemAction> newTakeItem(std::shared_ptr<Item> item, std::shared_ptr<Object> takeFrom) {
        return std::make_unique<TakeItemAction>(_game, _services, std::move(item), std::move(takeFrom));
    }

    std::unique_ptr<UnequipItemAction> newUnequipItem(std::shared_ptr<Item> item, bool instant) {
        return std::make_unique<UnequipItemAction>(_game, _services, std::move(item), instant);
    }

    std::unique_ptr<UnlockObjectAction> newUnlockObject(std::shared_ptr<Object> target) {
        return std::make_unique<UnlockObjectAction>(_game, _services, std::move(target));
    }

    std::unique_ptr<UseFeatAction> newUseFeat(FeatType feat, std::shared_ptr<Object> target) {
        return std::make_unique<UseFeatAction>(_game, _services, feat, std::move(target));
    }

    std::unique_ptr<UseSkillAction> newUseSkill(SkillType skill, std::shared_ptr<Object> target, int subSkill = 0, std::shared_ptr<Item> itemUsed = nullptr) {
        return std::make_unique<UseSkillAction>(_game, _services, skill, std::move(target), subSkill, std::move(itemUsed));
    }

    std::unique_ptr<UseTalentAtLocationAction> newUseTalentAtLocation(std::shared_ptr<Talent> chosenTalent, std::shared_ptr<Location> targetLocation) {
        return std::make_unique<UseTalentAtLocationAction>(_game, _services, std::move(chosenTalent), std::move(targetLocation));
    }

    std::unique_ptr<UseTalentOnObjectAction> newUseTalentOnObject(std::shared_ptr<Talent> chosenTalent, std::shared_ptr<Object> target) {
        return std::make_unique<UseTalentOnObjectAction>(_game, _services, std::move(chosenTalent), std::move(target));
    }

    std::unique_ptr<WaitAction> newWait(float seconds) {
        return std::make_unique<WaitAction>(_game, _services, seconds);
    }

private:
    Game &_game;
    ServicesView &_services;
};

} // namespace game

} // namespace reone
