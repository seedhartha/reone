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

#include "attack.h"
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
#include "forcefollowobject.h"
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

class ActionFactory {
public:
    std::unique_ptr<AttackAction> newAttack(std::shared_ptr<Object> object, float range = 1.0f, bool userAction = false, bool passive = false) {
        return std::make_unique<AttackAction>(*_game, std::move(object), range, userAction, passive);
    }

    std::unique_ptr<BarkStringAction> newBarkString() {
        return std::make_unique<BarkStringAction>(*_game);
    }

    std::unique_ptr<CastFakeSpellAtLocationAction> newCastFakeSpellAtLocation() {
        return std::make_unique<CastFakeSpellAtLocationAction>(*_game);
    }

    std::unique_ptr<CastFakeSpellAtObjectAction> newCastFakeSpellAtObject() {
        return std::make_unique<CastFakeSpellAtObjectAction>(*_game);
    }

    std::unique_ptr<CastSpellAtLocationAction> newCastSpellAtLocation() {
        return std::make_unique<CastSpellAtLocationAction>(*_game);
    }

    std::unique_ptr<CastSpellAtObjectAction> newCastSpellAtObject() {
        return std::make_unique<CastSpellAtObjectAction>(*_game);
    }

    std::unique_ptr<CloseDoorAction> newCloseDoor(std::shared_ptr<Object> object) {
        return std::make_unique<CloseDoorAction>(*_game, std::move(object));
    }

    std::unique_ptr<CommandAction> newDoCommand(std::shared_ptr<script::ExecutionContext> context) {
        return std::make_unique<CommandAction>(*_game, std::move(context));
    }

    std::unique_ptr<EquipItemAction> newEquipItem(std::shared_ptr<Item> item, int inventorySlot, bool instant) {
        return std::make_unique<EquipItemAction>(*_game, std::move(item), inventorySlot, instant);
    }

    std::unique_ptr<EquipMostDamagingMeleeAction> newEquipMostDamagingMelee() {
        return std::make_unique<EquipMostDamagingMeleeAction>(*_game);
    }

    std::unique_ptr<EquipMostDamagingRangedAction> newEquipMostDamagingRanged() {
        return std::make_unique<EquipMostDamagingRangedAction>(*_game);
    }

    std::unique_ptr<EquipMostEffectiveArmorAction> newEquipMostEffectiveArmor() {
        return std::make_unique<EquipMostEffectiveArmorAction>(*_game);
    }

    std::unique_ptr<FollowAction> newFollow(std::shared_ptr<Object> object, float distance) {
        return std::make_unique<FollowAction>(*_game, std::move(object), distance);
    }

    std::unique_ptr<FollowLeaderAction> newFollowLeader() {
        return std::make_unique<FollowLeaderAction>(*_game);
    }

    std::unique_ptr<FollowOwnerAction> newFollowOwner() {
        return std::make_unique<FollowOwnerAction>(*_game);
    }

    std::unique_ptr<ForceFollowObjectAction> newForceFollowObject() {
        return std::make_unique<ForceFollowObjectAction>(*_game);
    }

    std::unique_ptr<GiveItemAction> newGiveItem() {
        return std::make_unique<GiveItemAction>(*_game);
    }

    std::unique_ptr<InteractObjectAction> newInteractObject() {
        return std::make_unique<InteractObjectAction>(*_game);
    }

    std::unique_ptr<JumpToLocationAction> newJumpToLocation(std::shared_ptr<Location> location) {
        return std::make_unique<JumpToLocationAction>(*_game, std::move(location));
    }

    std::unique_ptr<JumpToObjectAction> newJumpToObject(std::shared_ptr<Object> object) {
        return std::make_unique<JumpToObjectAction>(*_game, std::move(object));
    }

    std::unique_ptr<LockObjectAction> newLockObject() {
        return std::make_unique<LockObjectAction>(*_game);
    }

    std::unique_ptr<MoveAwayFromLocationAction> newMoveAwayFromLocation() {
        return std::make_unique<MoveAwayFromLocationAction>(*_game);
    }

    std::unique_ptr<MoveAwayFromObject> newMoveAwayFromObject(std::shared_ptr<Object> fleeFrom, bool run, float range) {
        return std::make_unique<MoveAwayFromObject>(*_game, std::move(fleeFrom), run, range);
    }

    std::unique_ptr<MoveToLocationAction> newMoveToLocation(std::shared_ptr<Location> destination, bool run = false) {
        return std::make_unique<MoveToLocationAction>(*_game, std::move(destination), run);
    }

    std::unique_ptr<MoveToObjectAction> newMoveToObject(std::shared_ptr<Object> object, bool run, float distance) {
        return std::make_unique<MoveToObjectAction>(*_game, std::move(object), run, distance);
    }

    std::unique_ptr<MoveToPointAction> newMoveToPoint(glm::vec3 point) {
        return std::make_unique<MoveToPointAction>(*_game, std::move(point));
    }

    std::unique_ptr<OpenContainerAction> newOpenContainer(std::shared_ptr<Object> object) {
        return std::make_unique<OpenContainerAction>(*_game, std::move(object));
    }

    std::unique_ptr<OpenDoorAction> newOpenDoor(std::shared_ptr<Object> object) {
        return std::make_unique<OpenDoorAction>(*_game, std::move(object));
    }

    std::unique_ptr<OpenLockAction> newOpenLock(std::shared_ptr<Object> object) {
        return std::make_unique<OpenLockAction>(*_game, std::move(object));
    }

    std::unique_ptr<PauseConversationAction> newPauseConversation() {
        return std::make_unique<PauseConversationAction>(*_game);
    }

    std::unique_ptr<PickUpItemAction> newPickUpItem(std::shared_ptr<Item> item) {
        return std::make_unique<PickUpItemAction>(*_game, std::move(item));
    }

    std::unique_ptr<PlayAnimationAction> newPlayAnimation(AnimationType anim, float speed = 1.0f, float durationSeconds = 0.0f) {
        return std::make_unique<PlayAnimationAction>(*_game, anim, speed, durationSeconds);
    }

    std::unique_ptr<PutDownItemAction> newPutDownItem(std::shared_ptr<Item> item) {
        return std::make_unique<PutDownItemAction>(*_game, std::move(item));
    }

    std::unique_ptr<RandomWalkAction> newRandomWalk() {
        return std::make_unique<RandomWalkAction>(*_game);
    }

    std::unique_ptr<ResumeConversationAction> newResumeConversation() {
        return std::make_unique<ResumeConversationAction>(*_game);
    }

    std::unique_ptr<SpeakStringAction> newSpeakString(std::string stringToSpeak, TalkVolume talkVolume) {
        return std::make_unique<SpeakStringAction>(*_game, std::move(stringToSpeak), talkVolume);
    }

    std::unique_ptr<SpeakStringByStrRefAction> newSpeakStringByStrRef() {
        return std::make_unique<SpeakStringByStrRefAction>(*_game);
    }

    std::unique_ptr<StartConversationAction> newStartConversation(std::shared_ptr<Object> object, std::string dialogResRef, bool ignoreStartRange = false) {
        return std::make_unique<StartConversationAction>(*_game, std::move(object), std::move(dialogResRef), ignoreStartRange);
    }

    std::unique_ptr<SurrenderToEnemiesAction> newSurrenderToEnemies() {
        return std::make_unique<SurrenderToEnemiesAction>(*_game);
    }

    std::unique_ptr<SwitchWeaponsAction> newSwitchWeapons() {
        return std::make_unique<SwitchWeaponsAction>(*_game);
    }

    std::unique_ptr<TakeItemAction> newTakeItem() {
        return std::make_unique<TakeItemAction>(*_game);
    }

    std::unique_ptr<UnequipItemAction> newUnequipItem(std::shared_ptr<Item> item, bool instant) {
        return std::make_unique<UnequipItemAction>(*_game, std::move(item), instant);
    }

    std::unique_ptr<UnlockObjectAction> newUnlockObject() {
        return std::make_unique<UnlockObjectAction>(*_game);
    }

    std::unique_ptr<UseFeatAction> newUseFeat(std::shared_ptr<Object> object, FeatType feat) {
        return std::make_unique<UseFeatAction>(*_game, std::move(object), feat);
    }

    std::unique_ptr<UseSkillAction> newUseSkill(std::shared_ptr<Object> object, SkillType skill) {
        return std::make_unique<UseSkillAction>(*_game, std::move(object), skill);
    }

    std::unique_ptr<UseTalentAtLocationAction> newUseTalentAtLocation() {
        return std::make_unique<UseTalentAtLocationAction>(*_game);
    }

    std::unique_ptr<UseTalentOnObjectAction> newUseTalentOnObject() {
        return std::make_unique<UseTalentOnObjectAction>(*_game);
    }

    std::unique_ptr<WaitAction> newWait(float seconds) {
        return std::make_unique<WaitAction>(*_game, seconds);
    }

    void setGame(Game &game) { _game = &game; }

private:
    Game *_game {nullptr};
};

} // namespace game

} // namespace reone