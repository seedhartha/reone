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

namespace reone {

namespace game {

enum class ObjectType {
    Creature = 1,
    Item = 2,
    Trigger = 4,
    Door = 8,
    AreaOfEffect = 16,
    Waypoint = 32,
    Placeable = 64,
    Store = 128,
    Encounter = 256,
    Sound = 512,

    Module = 0x1000,
    Area = 0x1001,
    Camera = 0x1002,

    Invalid = 0x7fff
};

enum class Animation {
    // Looping

    LoopingPause = 0,
    LoopingPause2 = 1,
    LoopingListen = 2,
    LoopingMeditate = 3,
    LoopingWorship = 4,
    LoopingTalkNormal = 5,
    LoopingTalkPleading = 6,
    LoopingTalkForceful = 7,
    LoopingTalkLaughing = 8,
    LoopingTalkSad = 9,
    LoopingGetLow = 10,
    LoopingGetMid = 11,
    LoopingPauseTired = 12,
    LoopingPauseDrunk = 13,
    LoopingFlirt = 14,
    LoopingUseComputer = 15,
    LoopingDance = 16,
    LoopingDance1 = 17,
    LoopingHorror = 18,
    LoopingReady = 19,
    LoopingDeactivate = 20,
    LoopingSpasm = 21,
    LoopingSleep = 22,
    LoopingProne = 23,
    LoopingPause3 = 24,
    LoopingWeld = 25,
    LoopingDead = 26,
    LoopingTalkInjured = 27,
    LoopingListenInjured = 28,
    LoopingTreatInjured = 29,
    LoopingDeadProne = 30,
    LoopingKneelTalkAngry = 31,
    LoopingKneelTalkSad = 32,
    LoopingCheckBody = 33,
    LoopingUnlockDoor = 34,
    LoopingSitAndMeditate = 35,
    LoopingSitChair = 36,
    LoopingSitChairDrink = 37,
    LoopingSitChairPazak = 38,
    LoopingSitChairComp1 = 39,
    LoopingSitChairComp2 = 40,
    LoopingRage = 41,
    LoopingClosed = 43,
    LoopingStealth = 44,
    LoopingChokeWorking = 45,
    LoopingMeditateStand = 46,
    LoopingChoke = 116,

    // END Looping

    // Fire and forget

    FireForgetHeadTurnLeft = 100,
    FireForgetHeadTurnRight = 101,
    FireForgetPauseScratchHead = 102,
    FireForgetPauseBored = 103,
    FireForgetSalute = 104,
    FireForgetBow = 105,
    FireForgetGreeting = 106,
    FireForgetTaunt = 107,
    FireForgetVictory1 = 108,
    FireForgetVictory2 = 109,
    FireForgetVictory3 = 110,
    FireForgetInject = 112,
    FireForgetUseComputer = 113,
    FireForgetPersuade = 114,
    FireForgetActivate = 115,
    FireForgetThrowHigh = 117,
    FireForgetThrowLow = 118,
    FireForgetCustom01 = 119,
    FireForgetTreatInjured = 120,
    FireForgetForceCast = 121,
    FireForgetOpen = 122,
    FireForgetDiveRoll = 123,
    FireForgetScream = 124,

    // END Fire and forget

    // Placeable

    PlaceableActivate = 200,
    PlaceableDeactivate = 201,
    PlaceableOpen = 202,
    PlaceableClose = 203,
    PlaceableAnimloop01 = 204,
    PlaceableAnimloop02 = 205,
    PlaceableAnimloop03 = 206,
    PlaceableAnimloop04 = 207,
    PlaceableAnimloop05 = 208,
    PlaceableAnimloop06 = 209,
    PlaceableAnimloop07 = 210,
    PlaceableAnimloop08 = 211,
    PlaceableAnimloop09 = 212,
    PlaceableAnimloop10 = 213

    // END Placeable
};

} // namespace game

} // namespace reone
