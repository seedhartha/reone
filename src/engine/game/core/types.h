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

namespace reone {

namespace audio {

class AudioStream;

}

namespace game {

constexpr int kNpcPlayer = -1;
constexpr int kEngineTypeInvalid = -1;
constexpr float kDefaultFollowDistance = 5.0f;
constexpr char kObjectTagPlayer[] = "player";
constexpr float kDefaultRaycastDistance = 8.0f;
constexpr float kSelectionDistance = 8.0f;
constexpr char kSceneMain[] = "main";

enum class BackgroundType {
    None,
    Menu,
    Load,
    Computer0,
    Computer1
};

enum class CursorType {
    None,
    Default,
    Attack,
    Door,
    Talk,
    Pickup,
    DisableMine,
    RecoverMine
};

enum class WeaponType {
    None = 0,
    Melee = 1,
    Ranged = 4
};

enum class WeaponWield {
    None = 0,
    StunBaton = 1,
    SingleSword = 2,
    DoubleBladedSword = 3,
    BlasterPistol = 4,
    BlasterRifle = 5,
    HeavyWeapon = 6
};

enum class CreatureWieldType {
    None = 0,
    StunBaton = 1,
    SingleSword = 2,
    DoubleBladedSword = 3,
    DualSwords = 4,
    BlasterPistol = 5,
    DualPistols = 6,
    BlasterRifle = 7,
    HandToHand = 8,
    HeavyWeapon = 9
};

enum class DamageType {
    Bludgeoning = 1,
    Piercing = 2,
    Slashing = 4,
    Physical = 7,
    Universal = 8,
    Acid = 16,
    Cold = 32,
    LightSide = 64,
    Electrical = 128,
    Fire = 256,
    DarkSide = 512,
    Sonic = 1024,
    Ion = 2048,
    Blaster = 4096
};

enum class AttackResultType {
    Invalid = 0,
    HitSuccessful = 1,
    CriticalHit = 2,
    AutomaticHit = 3,
    Miss = 4,
    AttackResisted = 5,
    AttackFailed = 6,
    Parried = 8,
    Deflected = 9
};

enum class CameraStyleType {
    Default,
    Combat
};

enum class DurationType {
    Instant = 0,
    Temporary = 1,
    Permanent = 2
};

enum class ConversationType {
    Cinematic = 0,
    Computer = 1
};

enum class ComputerType {
    Normal = 0,
    Rakatan = 1
};

enum class SoundSetEntry {
    BattleCry1 = 0,
    BattleCry2 = 1,
    BattleCry3 = 2,
    BattleCry4 = 3,
    BattleCry5 = 4,
    BattleCry6 = 5,
    Select1 = 6,
    Select2 = 7,
    Select3 = 8,
    AttackGrunt1 = 9,
    AttackGrunt2 = 10,
    AttackGrunt3 = 11,
    PainGrunt1 = 12,
    PainGrunt2 = 13,
    LowHealth = 14,
    Dead = 15,
    CriticalHit = 16,
    TargetImmune = 17,
    LayMine = 18,
    DisarmMine = 19,
    BeginStealth = 20,
    BeginSearch = 21,
    BeginUnlock = 22,
    UnlockFailed = 23,
    UnlockSuccess = 24,
    SeparatedFromParty = 25,
    RejoinParty = 26,
    Poisoned = 27
};

enum class CameraType {
    FirstPerson,
    ThirdPerson,
    Static,
    Animated,
    Dialog
};

enum class EffectType {
    Invalid = 0,
    DamageResistance = 1,
    Regenerate = 3,
    DamageReduction = 7,
    TemporaryHitpoints = 9,
    Entangle = 11,
    Invulnerable = 12,
    Deaf = 13,
    Resurrection = 14,
    Immunity = 15,
    EnemyAttackBonus = 17,
    ArcaneSpellFailure = 18,
    AreaOfEffect = 20,
    Beam = 21,
    Charmed = 23,
    Confused = 24,
    Frightened = 25,
    Dominated = 26,
    Paralyze = 27,
    Dazed = 28,
    Stunned = 29,
    Sleep = 30,
    Poison = 31,
    Disease = 32,
    Curse = 33,
    Silence = 34,
    Turned = 35,
    Haste = 36,
    Slow = 37,
    AbilityIncrease = 38,
    AbilityDecrease = 39,
    AttackIncrease = 40,
    AttackDecrease = 41,
    DamageIncrease = 42,
    DamageDecrease = 43,
    DamageImmunityIncrease = 44,
    DamageImmunityDecrease = 45,
    ACIncrease = 46,
    ACDecrease = 47,
    MovementSpeedIncrease = 48,
    MovementSpeedDecrease = 49,
    SavingThrowIncrease = 50,
    SavingThrowDecrease = 51,
    ForceResistanceIncrease = 52,
    ForceResistanceDecrease = 53,
    SkillIncrease = 54,
    SkillDecrease = 55,
    Invisibility = 56,
    ImprovedInvisibility = 57,
    Darkness = 58,
    DispelMagicAll = 59,
    ElementalShield = 60,
    NegativeLevel = 61,
    Disguise = 62,
    Sanctuary = 63,
    TrueSeeing = 64,
    SeeInvisible = 65,
    TimeStop = 66,
    Blindness = 67,
    SpellLevelAbsorption = 68,
    DispelMagicBest = 69,
    Ultravision = 70,
    MissChance = 71,
    Concealment = 72,
    SpellImmunity = 73,
    AssuredHit = 74,
    Visual = 75,
    LightsaberThrow = 76,
    ForceJump = 77,
    AssuredDeflection = 78,
    DroidConfused = 79,
    MindTrick = 80,
    DroidScramble = 81,

    Damage = 0x100,
    Heal = 0x101,
    Death = 0x102,
    Knockdown = 0x103,
    TemporaryForcePoints = 0x104,
    Choke = 0x105,
    LinkEffects = 0x106,
    BodyFuel = 0x107,
    ForcePushTargeted = 0x108,
    DamageForcePoints = 0x109,
    HealForcePoints = 0x10a,
    HitPointsChangeWhenDying = 0x10b,
    DroidStun = 0x10c,
    ForcePushed = 0x10d,
    ForceResisted = 0x10e,
    ForceFizzle = 0x10f,
    ForceShield = 0x110,
    BlasterDeflectionIncrease = 0x111,
    BlasterDeflectionDecrease = 0x112,
    Horrified = 0x113,
    ModifyAttacks = 0x114,
    DamageShield = 0x115,
    ForceDrain = 0x116,
    PsychicStatic = 0x117,
    WhirlWind = 0x118,
    CutSceneHorrified = 0x119,
    CutSceneParalyze = 0x11a,
    CutSceneStunned = 0x11b,
    ForceBody = 0x11c,
    Fury = 0x11d,
    FPRegenModifier = 0x11e,
    VPRegenModifier = 0x11f,
    Crush = 0x120,
    ForceSight = 0x121,
    FactionModifier = 0x122
};

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

    All = 0x7fff,
    Invalid = 0x7fff
};

enum class AnimationType {
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
    PlaceableAnimloop10 = 213,

    // END Placeable

    Invalid = 32767
};

enum class CombatAnimation {
    None,
    Draw,
    Ready,
    Attack,
    Damage,
    Dodge,

    MeleeAttack,
    MeleeDamage,
    MeleeDodge,

    CinematicMeleeAttack,
    CinematicMeleeDamage,
    CinematicMeleeParry,

    BlasterAttack
};

enum class Gender {
    Male = 0,
    Female = 1,
    Both = 2,
    Other = 3,
    None = 4
};

enum class ClassType {
    Soldier = 0,
    Scout = 1,
    Scoundrel = 2,
    JediGuardian = 3,
    JediConsular = 4,
    JediSentinel = 5,
    CombatDroid = 6,
    ExpertDroid = 7,
    Minion = 8,
    TechSpecialist = 9,
    BountyHunter = 10,
    JediWeaponMaster = 11,
    JediMaster = 12,
    JediWatchman = 13,
    SithMarauder = 14,
    SithLord = 15,
    SithAssassin = 16,

    Invalid = 255
};

enum class Faction {
    Invalid = -1,
    Hostile1 = 1,
    Friendly1 = 2,
    Hostile2 = 3,
    Friendly2 = 4,
    Neutral = 5,
    Insane = 6,
    Tuskan = 7,
    GlobalXor = 8,
    Surrender1 = 9,
    Surrender2 = 10,
    Predator = 11,
    Prey = 12,
    Trap = 13,
    EndarSpire = 14,
    Rancor = 15,
    Gizka1 = 16,
    Gizka2 = 17,
    SelfLoathing = 21,
    OneOnOne = 22,
    PartyPuppet = 23
};

enum class Ability {
    Strength = 0,
    Dexterity = 1,
    Constitution = 2,
    Intelligence = 3,
    Wisdom = 4,
    Charisma = 5
};

enum class SkillType {
    Invalid = -1,
    ComputerUse = 0,
    Demolitions = 1,
    Stealth = 2,
    Awareness = 3,
    Persuade = 4,
    Repair = 5,
    Security = 6,
    TreatInjury = 7
};

enum class FeatType {
    Invalid = 0,
    AdvancedJediDefense = 1,
    AdvancedGuardStance = 2,
    Ambidexterity = 3,
    ArmourProfHeavy = 4,
    ArmourProfLight = 5,
    ArmourProfMedium = 6,
    Cautious = 7,
    CriticalStrike = 8,
    DoubleWeaponFighting = 9,
    Empathy = 10,
    Flurry = 11,
    GearHead = 12,
    GreatFortitude = 13,
    ImplantLevel1 = 14,
    ImplantLevel2 = 15,
    ImplantLevel3 = 16,
    ImprovedPowerAttack = 17,
    ImprovedPowerBlast = 18,
    ImprovedCriticalStrike = 19,
    ImprovedSniperShot = 20,
    IronWill = 21,
    LightningReflexes = 22,
    MasterJediDefense = 24,
    MasterGuardStance = 25,
    MultiShot = 26,
    Perceptive = 27,
    PowerAttack = 28,
    PowerBlast = 29,
    RapidShot = 30,
    SniperShot = 31,
    WeaponFocusBlaster = 32,
    WeaponFocusBlasterRifle = 33,
    WeaponFocusGrenade = 34,
    WeaponFocusHeavyWeapons = 35,
    WeaponFocusLightsaber = 36,
    WeaponFocusMeleeWeapons = 37,
    WeaponFocusSimpleWeapons = 38,
    WeaponProficiencyBlaster = 39,
    WeaponProficiencyBlasterRifle = 40,
    WeaponProficiencyGrenade = 41,
    WeaponProficiencyHeavyWeapons = 42,
    WeaponProficiencyLightsaber = 43,
    WeaponProficiencyMeleeWeapons = 44,
    WeaponProficiencySimpleWeapons = 45,
    WeaponSpecializationBlaster = 46,
    WeaponSpecializationBlasterRifle = 47,
    WeaponSpecializationGrenade = 48,
    WeaponSpecializationHeavyWeapons = 49,
    WeaponSpecializationLightsaber = 50,
    WeaponSpecializationMeleeWeapons = 51,
    WeaponSpecializationSimpleWeapons = 52,
    WhirlwindAttack = 53,
    GuardStance = 54,
    JediDefense = 55,
    UncannyDodge1 = 56,
    UncannyDodge2 = 57,
    SkillFocusComputerUse = 58,
    SneakAttack1d6 = 60,
    SneakAttack2d6 = 61,
    SneakAttack3d6 = 62,
    SneakAttack4d6 = 63,
    SneakAttack5d6 = 64,
    SneakAttack6d6 = 65,
    SneakAttack7d6 = 66,
    SneakAttack8d6 = 67,
    SneakAttack9d6 = 68,
    SneakAttack10d6 = 69,
    SkillFocusDemolitions = 70,
    SkillFocusStealth = 71,
    SkillFocusAwareness = 72,
    SkillFocusPersuade = 73,
    SkillFocusRepair = 74,
    SkillFocusSecurity = 75,
    SkillFocusTreatInjuury = 76,
    MasterSniperShot = 77,
    DroidUpgrade1 = 78,
    DroidUpgrade2 = 79,
    DroidUpgrade3 = 80,
    MasterCriticalStrike = 81,
    MasterPowerBlast = 82,
    MasterPowerAttack = 83,
    Toughness = 84,
    AdvancedDoubleWeaponFighting = 85,
    ForceFocusAlter = 86,
    ForceFocusControl = 87,
    ForceFocusSense = 88,
    ForceFocusAdvanced = 89,
    ForceFocusMastery = 90,
    ImprovedFlurry = 91,
    ImprovedRapidShot = 92,
    ProficiencyAll = 93,
    BattleMeditation = 94,

    // TSL

    Evasion = 125,
    Targeting1 = 126,
    Targeting2 = 127,
    Targeting3 = 128,
    Targeting4 = 129,
    Targeting5 = 130,
    Targeting6 = 131,
    Targeting7 = 132,
    Targeting8 = 133,
    Targeting9 = 134,
    Targeting10 = 135,
    CloseCombat = 139,
    ImprovedCloseCombat = 140,
    ImprovedForceCcamouflage = 141,
    MasterForceCamouflage = 142,
    RegenerateForcePoints = 143,
    DarkSideCorruption = 149,
    IgnorePain1 = 150,
    IgnorePain2 = 151,
    IgnorePain3 = 152,
    IncreaseCombatDamage1 = 153,
    IncreaseCombatDamage2 = 154,
    IncreaseCombatDamage3 = 155,
    SuperiorWeaponFocusLightsaber1 = 156,
    SuperiorWeaponFocusLightsaber2 = 157,
    SuperiorWeaponFocusLightsaber3 = 158,
    SuperiorWeaponFocusTwoWeapon1 = 159,
    SuperiorWeaponFocusTwoWeapon2 = 160,
    SuperiorWeaponFocusTwoWeapon3 = 161,
    LightSideEnlightenment = 167,
    Deflect = 168,
    InnerStrength1 = 169,
    InnerStrength2 = 170,
    InnerStrength3 = 171,
    IncreaseMeleeDamage1 = 172,
    IncreaseMeleeDamage2 = 173,
    IncreaseMeleeDamage3 = 174,
    Craft = 175,
    MastercraftWeapons1 = 176,
    MastercraftWeapons2 = 177,
    MastercraftWeapons3 = 178,
    MastercraftArmor1 = 179,
    MastercraftArmor2 = 180,
    MastercraftArmor3 = 181,
    DroidInterface = 182,
    ClassSkillAwareness = 183,
    ClassSkillComputerUse = 184,
    ClassSkillDemolitions = 185,
    ClassSkillRepair = 186,
    ClassSkillSecurity = 187,
    ClassSkillStealth = 188,
    ClassSkillTreatInjury = 189,
    DualStrike = 190,
    ImprovedDualStrike = 191,
    MasterDualStrike = 192,
    FinesseLightsabers = 193,
    FinesseMeleeWeapons = 194,
    Mobility = 195,
    RegenerateVitalityPoints = 196,
    StealthRun = 197,
    KineticCombat = 198,
    Survival = 199,
    MandalorianCourage = 200,
    PersonalCloakingShield = 201,
    Mentor = 202,
    ImplantSwitching = 203,
    Spirit = 204,
    ForceChain = 205,
    WarVeteran = 206,
    FightingSpirit = 236,
    HeroicResolve = 237,
    PreciseShot = 240,
    ImprovedPreciseShot = 241,
    MasterPreciseShot = 242,
    PreciseShotIV = 243,
    PreciseShotV = 244

    // END TSL
};

enum class ForcePower {
    All = -1,
    MasterAlter = 0,
    MasterControl = 1,
    MasterSense = 2,
    ForceJumpAdvanced = 3,
    LightSaberThrowAdvanced = 4,
    RegnerationAdvanced = 5,
    AffectMind = 6,
    Affliction = 7,
    SpeedBurst = 8,
    Choke = 9,
    Cure = 10,
    DeathField = 11,
    DroidDisable = 12,
    DroidDestroy = 13,
    Dominate = 14,
    DrainLife = 15,
    Fear = 16,
    ForceArmor = 17,
    ForceAura = 18,
    ForceBreach = 19,
    ForceImmunity = 20,
    ForceJump = 21,
    ForceMind = 22,
    ForcePush = 23,
    ForceShield = 24,
    ForceStorm = 25,
    ForceWave = 26,
    ForceWhirlwind = 27,
    Heal = 28,
    Hold = 29,
    Horror = 30,
    Insanity = 31,
    Kill = 32,
    KnightMind = 33,
    KnightSpeed = 34,
    Lightning = 35,
    MindMastery = 36,
    SpeedMastery = 37,
    Plague = 38,
    Regeneration = 39,
    ResistColdHeatEnergy = 40,
    ResistForce = 41,
    ResistPoisonDiseaseSonic = 42,
    Shock = 43,
    Sleep = 44,
    Slow = 45,
    Stun = 46,
    DroidStun = 47,
    SupressForce = 48,
    LightSaberThrow = 49,
    Wound = 50,
    BattleMeditation = 51,
    BodyFuel = 52,
    CombatRegeneration = 53,
    WarriorStance = 54,
    SentinelStance = 55,
    DominateMind = 56,
    PsychicStance = 57,
    CatharReflexes = 58,
    EnhancedSenses = 59,
    Camoflage = 60,
    Taunt = 61,
    WhirlingDervish = 62,
    Rage = 63,

    // TSL

    MasterEnergyResistance = 133,
    MasterHeal = 134,
    ForceBarrier = 135,
    ImprovedForceBarrier = 136,
    MasterForceBarrier = 137,
    BattleMeditationPC = 138,
    ImprovedBattleMeditationPC = 139,
    MasterBattleMeditationPC = 140,
    BatMedEnemy = 141,
    ImpBatMedEnemy = 142,
    MasBatMedEnemy = 143,
    CrushOppositionI = 144,
    CrushOppositionII = 145,
    CrushOppositionIII = 146,
    CrushOppositionIV = 147,
    CrushOppositionV = 148,
    CrushOppositionVI = 149,
    ForceBody = 150,
    ImprovedForceBody = 151,
    MasterForceBody = 152,
    DrainForce = 153,
    ImprovedDrainForce = 154,
    MasterDrainForce = 155,
    ForceCamouflage = 156,
    ImprovedForceCamouflage = 157,
    MasterForceCamouflage = 158,
    ForceScream = 159,
    ImprovedForceScream = 160,
    MasterForceScream = 161,
    ForceRepulsion = 162,
    ForceRedirection = 163,
    Fury = 164,
    ImprovedFury = 165,
    MasterFury = 166,
    InspireFollowersI = 167,
    InspireFollowersII = 168,
    InspireFollowersIII = 169,
    InspireFollowersIV = 170,
    InspireFollowersV = 171,
    InspireFollowersVI = 172,
    Revitalize = 173,
    ImprovedRevitalize = 174,
    MasterRevitalize = 175,
    ForceSight = 176,
    ForceCrush = 177,
    Precognition = 178,
    BattlePrecognition = 179,
    ForceEnlightenment = 180,
    MindTrick = 181,
    Confusion = 200,
    BeastTrick = 182,
    BeastConfusion = 184,
    DroidTrick = 201,
    DroidConfusion = 269,
    BreathControl = 270,
    WookieeRageI = 271,
    WookieeRageII = 272,
    WookieeRageIII = 273

    // END TSL
};

enum class CreatureType {
    RacialType = 0,
    PlayerChar = 1,
    Class = 2,
    Reputation = 3,
    IsAlive = 4,
    HasSpellEffect = 5,
    DoesNotHaveSpellEffect = 6,
    Perception = 7
};

enum class ReputationType {
    Friend = 0,
    Enemy = 1,
    Neutral = 2
};

enum class PerceptionType {
    SeenAndHeard = 0,
    NotSeenAndNotHeard = 1,
    HeardAndNotSeen = 2,
    SeenAndNotHeard = 3,
    NotHeard = 4,
    Heard = 5,
    NotSeen = 6,
    Seen = 7
};

enum class RacialType {
    Unknown = 0,
    Elf = 1,
    Gnome = 2,
    Halfling = 3,
    Halfelf = 4,
    Droid = 5,
    Human = 6,
    All = 7,
    Invalid = 8
};

enum class Subrace {
    None = 0,
    Wookie = 1,
    Beast = 2
};

enum class PartyAIStyle {
    Aggressive = 0,
    Defensive = 1,
    Passive = 2
};

enum class NPCAIStyle {
    DefaultAttack = 0,
    RangedAttack = 1,
    MeleeAttack = 2,
    Aid = 3,
    GrenadeThrower = 4,
    JediSupport = 5,

    // TSL
    Healer = 6,           // no fighting, just heals
    Skirmish = 7,         // this AI moves and shoots
    Turtle = 8,           // turtles then unleashes charged attack
    PartyAggro = 9,       // all out attacking party member
    PartyDefense = 10,    // attacks, but doesn't stray to far
    PartyRanged = 11,     // doesn't run around a lot, but does heal
    PartyStationary = 12, // uses healing and defensive force powers
    PartySupport = 13,    // revisiting party AI stuff
    PartyRemote = 14,     // combat AI for the floating remote
    MonsterPowers = 15    // combat AI for the Malachor Storm Beasts and other ability-using monsters
};

enum class ActionType {
    MoveToPoint = 0,
    PickUpItem = 1,
    DropItem = 2,
    AttackObject = 3,
    CastSpell = 4,
    OpenDoor = 5,
    CloseDoor = 6,
    DialogObject = 7,
    DisableTrap = 8,
    RecoverTrap = 9,
    FlagTrap = 10,
    ExamineTrap = 11,
    SetTrap = 12,
    OpenLock = 13,
    Lock = 14,
    UseObject = 15,
    AnimalEmpathy = 16,
    Rest = 17,
    Taunt = 18,
    ItemCastSpell = 19,
    CounterSpell = 31,
    Heal = 33,
    PickPocket = 34,
    Follow = 35,
    Wait = 36,
    Sit = 37,
    FollowLeader = 38,
    FollowOwner = 43,

    DoCommand = 0x1000,
    StartConversation = 0x1001,
    PauseConversation = 0x1002,
    ResumeConversation = 0x1003,
    MoveToObject = 0x1004,
    OpenContainer = 0x1005,
    JumpToObject = 0x1006,
    JumpToLocation = 0x1007,
    RandomWalk = 0x1008,
    MoveToLocation = 0x1009,
    MoveAwayFromObject = 0x100a,
    EquipItem = 0x100b,
    UnequipItem = 0x100c,
    SpeakString = 0x100d,
    PlayAnimation = 0x100e,
    CastSpellAtObject = 0x100f,
    GiveItem = 0x1010,
    TakeItem = 0x1011,
    ForceFollowObject = 0x1012,
    CastSpellAtLocation = 0x1013,
    SpeakStringByStrRef = 0x1014,
    UseFeat = 0x1015,
    UseSkill = 0x1016,
    UseTalentOnObject = 0x1017,
    UseTalentAtLocation = 0x1018,
    InteractObject = 0x1019,
    MoveAwayFromLocation = 0x101a,
    SurrenderToEnemies = 0x101b,
    EquipMostDamagingMelee = 0x101c,
    EquipMostDamagingRanged = 0x101d,
    EquipMostEffectiveArmor = 0x101e,
    CastFakeSpellAtObject = 0x1021,
    CastFakeSpellAtLocation = 0x1022,
    BarkString = 0x1023,
    SwitchWeapons = 0x1024,
    PutDownItem = 0x1025,

    Invalid = 0xffff,
    QueueEmpty = 0xfffe
};

enum class TalkVolume {
    Talk = 0,
    Whisper = 1,
    Shout = 2,
    SilentTalk = 3,
    SilentShout = 4
};

enum class ProjectilePathType {
    Default = 0,
    Homing = 1,
    Ballistic = 2,
    HighBallistic = 3,
    Accelerating = 4
};

enum class SubSkill {
    FlagTrap = 100,
    RecoverTrap = 101,
    ExamineTrap = 102
};

enum class TalentType {
    Force = 0,
    Spell = 0,
    Feat = 1,
    Skill = 2,
    Invalid = 3
};

enum class InGameMenuTab {
    None,
    Equipment,
    Inventory,
    Character,
    Abilities,
    Messages,
    Journal,
    Map,
    Options
};

enum class SaveLoadMode {
    Save,
    LoadFromMainMenu,
    LoadFromInGame
};

struct InventorySlot {
    static constexpr int head = 0;
    static constexpr int body = 1;
    static constexpr int hands = 3;
    static constexpr int rightWeapon = 4;
    static constexpr int leftWeapon = 5;
    static constexpr int leftArm = 7;
    static constexpr int rightArm = 8;
    static constexpr int implant = 9;
    static constexpr int belt = 10;
    static constexpr int cWeaponL = 14;
    static constexpr int cWeaponR = 15;
    static constexpr int cWeaponB = 16;
    static constexpr int cArmour = 17;
    static constexpr int rightWeapon2 = 18;
    static constexpr int leftWeapon2 = 19;
};

struct PartySelectionContext {
    std::string exitScript;
    int forceNpc1 {-1};
    int forceNpc2 {-2};
};

typedef std::unordered_map<SoundSetEntry, std::shared_ptr<audio::AudioStream>> SoundSet;

} // namespace game

} // namespace reone
