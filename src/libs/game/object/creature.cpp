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

#include "reone/game/object/creature.h"

#include "reone/audio/di/services.h"
#include "reone/audio/player.h"
#include "reone/game/action.h"
#include "reone/game/action/attackobject.h"
#include "reone/game/animationutil.h"
#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/footstepsounds.h"
#include "reone/game/game.h"
#include "reone/game/portraits.h"
#include "reone/game/script/runner.h"
#include "reone/game/surfaces.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/textureregistry.h"
#include "reone/resource/2da.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/soundsets.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/types.h"
#include "reone/script/types.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"
#include "reone/system/timer.h"

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr int kStrRefRemains = 38151;
static constexpr float kKeepPathDuration = 1000.0f;

static std::string g_talkDummyNode("talkdummy");

static const std::string g_headHookNode("headhook");
static const std::string g_maskHookNode("gogglehook");
static const std::string g_rightHandNode("rhand");
static const std::string g_leftHandNode("lhand");

void Creature::Path::selectNextPoint() {
    size_t pointCount = points.size();
    if (pointIdx < pointCount) {
        pointIdx++;
    }
}

void Creature::loadFromGIT(const resource::generated::GIT_Creature_List &git) {
    std::string templateResRef(boost::to_lower_copy(git.TemplateResRef));
    loadFromBlueprint(templateResRef);
    loadTransformFromGIT(git);
}

void Creature::loadFromBlueprint(const std::string &resRef) {
    auto utc = _services.resource.gffs.get(resRef, ResType::Utc);
    if (!utc) {
        return;
    }
    loadUTC(resource::generated::parseUTC(*utc));
    loadAppearance();
}

void Creature::loadAppearance() {
    std::shared_ptr<TwoDa> appearances(_services.resource.twoDas.get("appearance"));
    if (!appearances) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }

    _modelType = parseModelType(appearances->getString(_appearance, "modeltype"));
    _walkSpeed = appearances->getFloat(_appearance, "walkdist", 1.0f);
    _runSpeed = appearances->getFloat(_appearance, "rundist", 1.0f);
    _footstepType = appearances->getInt(_appearance, "footsteptype", -1);
    _envmap = boost::to_lower_copy(appearances->getString(_appearance, "envmap"));

    if (_portraitId > 0) {
        _portrait = _services.game.portraits.getTextureByIndex(_portraitId);
    } else {
        _portrait = _services.game.portraits.getTextureByAppearance(_appearance);
    }

    auto modelSceneNode = buildModel();
    if (modelSceneNode) {
        finalizeModel(*modelSceneNode);
        _sceneNode = std::move(modelSceneNode);
        _sceneNode->setUser(*this);
        _sceneNode->setLocalTransform(_transform);
    }

    _animDirty = true;
}

Creature::ModelType Creature::parseModelType(const std::string &s) const {
    if (s == "S" || s == "L") {
        return ModelType::Creature;
    } else if (s == "F") {
        return ModelType::Droid;
    } else if (s == "B") {
        return ModelType::Character;
    }

    throw std::invalid_argument(str(boost::format("Model type '%s' is not supported") % s));
}

void Creature::updateModel() {
    if (!_sceneNode) {
        return;
    }
    auto bodyModelName = getBodyModelName();
    if (bodyModelName.empty()) {
        return;
    }
    auto replacement = _services.resource.models.get(bodyModelName);
    if (!replacement) {
        return;
    }
    auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
    model->setModel(*replacement);
    finalizeModel(*model);
    if (!_stunt) {
        model->setLocalTransform(_transform);
    }
    _animDirty = true;
}

void Creature::loadTransformFromGIT(const resource::generated::GIT_Creature_List &git) {
    _position[0] = git.XPosition;
    _position[1] = git.YPosition;
    _position[2] = git.ZPosition;

    float cosine = git.XOrientation;
    float sine = git.YOrientation;
    _orientation = glm::quat(glm::vec3(0.0f, 0.0f, -glm::atan(cosine, sine)));

    updateTransform();
}

bool Creature::isSelectable() const {
    bool hasDropableItems = false;
    for (auto &item : _items) {
        if (item->isDropable()) {
            hasDropableItems = true;
            break;
        }
    }
    return !_dead || hasDropableItems;
}

void Creature::update(float dt) {
    Object::update(dt);
    updateModelAnimation();
    updateHealth();
    updateCombat(dt);

    if (_audioSourceVoice) {
        _audioSourceVoice->update();
    }
    if (_audioSourceFootstep) {
        _audioSourceFootstep->update();
    }
}

void Creature::updateModelAnimation() {
    auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
    if (!model)
        return;

    if (_animFireForget) {
        if (!model->isAnimationFinished())
            return;

        _animFireForget = false;
        _animDirty = true;
    }
    if (!_animDirty)
        return;

    std::shared_ptr<Animation> anim;
    std::shared_ptr<Animation> talkAnim;

    switch (_movementType) {
    case MovementType::Run:
        anim = model->model().getAnimation(getRunAnimation());
        break;
    case MovementType::Walk:
        anim = model->model().getAnimation(getWalkAnimation());
        break;
    default:
        if (_dead) {
            anim = model->model().getAnimation(getDeadAnimation());
        } else if (_talking) {
            anim = model->model().getAnimation(getTalkNormalAnimation());
            talkAnim = model->model().getAnimation(getHeadTalkAnimation());
        } else {
            anim = model->model().getAnimation(getPauseAnimation());
        }
        break;
    }

    if (talkAnim) {
        model->playAnimation(*anim, nullptr, AnimationProperties::fromFlags(AnimationFlags::loopOverlay | AnimationFlags::propagate));
        model->playAnimation(*talkAnim, _lipAnimation.get(), AnimationProperties::fromFlags(AnimationFlags::loopOverlay | AnimationFlags::propagate));
    } else {
        model->playAnimation(*anim, nullptr, AnimationProperties::fromFlags(AnimationFlags::loopBlend | AnimationFlags::propagate));
    }

    _animDirty = false;
}

void Creature::updateHealth() {
    if (_currentHitPoints > 0 || _immortal || _dead)
        return;

    die();
}

void Creature::updateCombat(float dt) {
    _combatState.deactivationTimer.update(dt);
    if (_combatState.deactivationTimer.elapsed()) {
        _combatState.active = false;
        _combatState.debilitated = false;
    }
}

void Creature::clearAllActions() {
    Object::clearAllActions();
    setMovementType(MovementType::None);
}

void Creature::playAnimation(AnimationType type, AnimationProperties properties) {
    // If animation is looping by type and duration is -1.0, set flags accordingly
    bool looping = isAnimationLooping(type) && properties.duration == -1.0f;
    if (looping) {
        properties.flags |= AnimationFlags::loop;
    }

    std::string animName(getAnimationName(type));
    if (animName.empty())
        return;

    playAnimation(animName, std::move(properties));
}

void Creature::playAnimation(const std::string &name, AnimationProperties properties) {
    bool fireForget = !(properties.flags & AnimationFlags::loop);

    doPlayAnimation(fireForget, [&]() {
        auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
        if (model) {
            model->playAnimation(name, nullptr, properties);
        }
    });
}

void Creature::doPlayAnimation(bool fireForget, const std::function<void()> &callback) {
    if (!_sceneNode || _movementType != MovementType::None)
        return;

    callback();

    if (fireForget) {
        _animFireForget = true;
    }
}

void Creature::playAnimation(const std::shared_ptr<Animation> &anim, AnimationProperties properties) {
    bool fireForget = !(properties.flags & AnimationFlags::loop);

    doPlayAnimation(fireForget, [&]() {
        auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
        if (model) {
            model->playAnimation(*anim, nullptr, properties);
        }
    });
}

void Creature::playAnimation(CombatAnimation anim, CreatureWieldType wield, int variant) {
    std::string animName(getAnimationName(anim, wield, variant));
    if (!animName.empty()) {
        playAnimation(animName, AnimationProperties::fromFlags(AnimationFlags::blend));
    }
}

bool Creature::equip(const std::string &resRef) {
    std::shared_ptr<Item> item = _game.newItem();
    item->loadFromBlueprint(resRef);

    bool equipped = false;

    if (item->isEquippable(InventorySlot::body)) {
        equipped = equip(InventorySlot::body, item);
    } else if (item->isEquippable(InventorySlot::rightWeapon)) {
        equipped = equip(InventorySlot::rightWeapon, item);
    }

    return equipped;
}

bool Creature::equip(int slot, const std::shared_ptr<Item> &item) {
    if (!item->isEquippable(slot)) {
        return false;
    }

    _equipment[slot] = item;
    item->setEquipped(true);

    if (_sceneNode) {
        updateModel();

        if (slot == InventorySlot::rightWeapon) {
            auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
            auto weapon = static_cast<ModelSceneNode *>(model->getAttachment("rhand"));
            if (weapon && weapon->model().classification() == MdlClassification::lightsaber) {
                weapon->playAnimation("powerup");
            }
        }
    }

    return true;
}

void Creature::unequip(const std::shared_ptr<Item> &item) {
    for (auto &equipped : _equipment) {
        if (equipped.second != item) {
            continue;
        }
        item->setEquipped(false);
        _equipment.erase(equipped.first);
        if (_sceneNode) {
            updateModel();
        }
        break;
    }
}

std::shared_ptr<Item> Creature::getEquippedItem(int slot) const {
    auto equipped = _equipment.find(slot);
    return equipped != _equipment.end() ? equipped->second : nullptr;
}

bool Creature::isSlotEquipped(int slot) const {
    return _equipment.find(slot) != _equipment.end();
}

void Creature::setMovementType(MovementType type) {
    if (_movementType == type)
        return;

    _movementType = type;
    _animDirty = true;
    _animFireForget = false;
}

void Creature::setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound) {
    int pointIdx = 0;
    if (_path) {
        bool lastPointReached = _path->pointIdx == _path->points.size();
        if (lastPointReached) {
            float nearestDist = INFINITY;
            for (int i = 0; i < points.size(); ++i) {
                float dist = glm::distance2(_path->destination, points[i]);
                if (dist < nearestDist) {
                    nearestDist = dist;
                    pointIdx = i;
                }
            }
        } else {
            const glm::vec3 &nextPoint = _path->points[_path->pointIdx];
            for (int i = 0; i < points.size(); ++i) {
                if (points[i] == nextPoint) {
                    pointIdx = i;
                    break;
                }
            }
        }
    }
    auto path = std::make_unique<Path>();
    path->destination = dest;
    path->points = points;
    path->timeFound = timeFound;
    path->pointIdx = pointIdx;

    _path = std::move(path);
}

void Creature::clearPath() {
    _path.reset();
}

glm::vec3 Creature::getSelectablePosition() const {
    auto model = std::static_pointer_cast<ModelSceneNode>(_sceneNode);
    if (!model) {
        return _position;
    }
    if (_dead) {
        return model->getWorldCenterOfAABB();
    }
    auto headModel = static_cast<ModelSceneNode *>(model->getAttachment(g_headHookNode));
    if (headModel) {
        auto talkDummy = headModel->getNodeByName(g_talkDummyNode);
        return talkDummy ? talkDummy->getOrigin() : headModel->getWorldCenterOfAABB();
    } else {
        auto talkDummy = model->getNodeByName(g_talkDummyNode);
        return talkDummy ? talkDummy->getOrigin() : model->getWorldCenterOfAABB();
    }
}

float Creature::getAttackRange() const {
    float result = kDefaultAttackRange;

    std::shared_ptr<Item> item(getEquippedItem(InventorySlot::rightWeapon));
    if (item && item->attackRange() > kDefaultAttackRange) {
        result = item->attackRange();
    }

    return result;
}

bool Creature::isLevelUpPending() const {
    return _xp >= getNeededXP();
}

int Creature::getNeededXP() const {
    int level = _attributes.getAggregateLevel();
    return level * (level + 1) * 500;
}

void Creature::runSpawnScript() {
    if (!_onSpawn.empty()) {
        _game.scriptRunner().run(_onSpawn, _id, kObjectInvalid);
    }
}

void Creature::runEndRoundScript() {
    if (!_onEndRound.empty()) {
        _game.scriptRunner().run(_onEndRound, _id, kObjectInvalid);
    }
}

void Creature::giveXP(int amount) {
    _xp += amount;
}

void Creature::playSound(SoundSetEntry entry, bool positional) {
    if (!_soundSet) {
        return;
    }
    auto maybeSound = _soundSet->find(entry);
    if (maybeSound == _soundSet->end()) {
        return;
    }
    glm::vec3 position(_position + 1.7f);
    _audioSourceVoice = _services.audio.player.play(maybeSound->second, AudioType::Sound, false, 1.0f, positional, position);
}

void Creature::die() {
    _currentHitPoints = 0;
    _dead = true;
    _name = _services.resource.strings.getText(kStrRefRemains);

    debug(boost::format("Creature %s is dead") % _tag);

    playSound(SoundSetEntry::Dead);
    playAnimation(getDieAnimation());
    runDeathScript();
}

void Creature::runDeathScript() {
    if (!_onDeath.empty()) {
        _game.scriptRunner().run(_onDeath, _id, kObjectInvalid);
    }
}

CreatureWieldType Creature::getWieldType() const {
    auto rightWeapon = getEquippedItem(InventorySlot::rightWeapon);
    auto leftWeapon = getEquippedItem(InventorySlot::leftWeapon);

    if (rightWeapon && leftWeapon) {
        return (rightWeapon->weaponWield() == WeaponWield::BlasterPistol) ? CreatureWieldType::DualPistols : CreatureWieldType::DualSwords;
    } else if (rightWeapon) {
        switch (rightWeapon->weaponWield()) {
        case WeaponWield::SingleSword:
            return CreatureWieldType::SingleSword;
        case WeaponWield::DoubleBladedSword:
            return CreatureWieldType::DoubleBladedSword;
        case WeaponWield::BlasterPistol:
            return CreatureWieldType::BlasterPistol;
        case WeaponWield::BlasterRifle:
            return CreatureWieldType::BlasterRifle;
        case WeaponWield::HeavyWeapon:
            return CreatureWieldType::HeavyWeapon;
        case WeaponWield::StunBaton:
        default:
            return CreatureWieldType::StunBaton;
        }
    }

    return CreatureWieldType::HandToHand;
}

void Creature::startTalking(const std::shared_ptr<LipAnimation> &animation) {
    if (!_talking || _lipAnimation != animation) {
        _lipAnimation = animation;
        _talking = true;
        _animDirty = true;
    }
}

void Creature::stopTalking() {
    if (_talking || _lipAnimation) {
        _lipAnimation.reset();
        _talking = false;
        _animDirty = true;
    }
}

void Creature::onObjectSeen(const std::shared_ptr<Object> &object) {
    _perception.seen.insert(object);
    _perception.lastPerception = PerceptionType::Seen;
    _perception.lastPerceived = object;
    runOnNoticeScript();
}

void Creature::runOnNoticeScript() {
    if (!_onNotice.empty()) {
        _game.scriptRunner().run(_onNotice, _id, _perception.lastPerceived->id());
    }
}

void Creature::onObjectVanished(const std::shared_ptr<Object> &object) {
    _perception.seen.erase(object);
    _perception.lastPerception = PerceptionType::NotSeen;
    _perception.lastPerceived = object;
    runOnNoticeScript();
}

void Creature::onObjectHeard(const std::shared_ptr<Object> &object) {
    _perception.heard.insert(object);
    _perception.lastPerception = PerceptionType::Heard;
    _perception.lastPerceived = object;
    runOnNoticeScript();
}

void Creature::onObjectInaudible(const std::shared_ptr<Object> &object) {
    _perception.heard.erase(object);
    _perception.lastPerception = PerceptionType::NotHeard;
    _perception.lastPerceived = object;
    runOnNoticeScript();
}

void Creature::activateCombat() {
    _combatState.active = true;
}

void Creature::deactivateCombat(float delay) {
    if (_combatState.active) {
        _combatState.deactivationTimer.reset(delay);
    }
}

bool Creature::isTwoWeaponFighting() const {
    return static_cast<bool>(getEquippedItem(InventorySlot::leftWeapon));
}

std::shared_ptr<Object> Creature::getAttemptedAttackTarget() const {
    auto action = getCurrentAction();
    if (!action) {
        return nullptr;
    }
    std::shared_ptr<Object> target;
    switch (action->type()) {
    case ActionType::AttackObject:
        target = static_cast<AttackObjectAction *>(action.get())->attackee();
        break;
    default:
        break;
    }
    return target;
}

int Creature::getAttackBonus(bool offHand) const {
    auto rightWeapon(getEquippedItem(InventorySlot::rightWeapon));
    auto leftWeapon(getEquippedItem(InventorySlot::leftWeapon));
    auto &weapon = offHand ? leftWeapon : rightWeapon;

    int modifier;
    if (weapon && weapon->isRanged()) {
        modifier = _attributes.getAbilityModifier(Ability::Dexterity);
    } else {
        modifier = _attributes.getAbilityModifier(Ability::Strength);
    }

    int penalty;
    if (rightWeapon && leftWeapon) {
        // TODO: support Dueling and Two-Weapon Fighting feats
        penalty = offHand ? 10 : 6;
    } else {
        penalty = 0;
    }

    return _attributes.getAggregateAttackBonus() + modifier - penalty;
}

int Creature::getDefense() const {
    return _attributes.getDefense();
}

void Creature::getMainHandDamage(int &min, int &max) const {
    getWeaponDamage(InventorySlot::rightWeapon, min, max);
}

void Creature::getWeaponDamage(int slot, int &min, int &max) const {
    auto weapon = getEquippedItem(slot);

    if (!weapon) {
        min = 1;
        max = 1;
    } else {
        min = weapon->numDice();
        max = weapon->numDice() * weapon->dieToRoll();
    }

    int modifier;
    if (weapon && weapon->isRanged()) {
        modifier = _attributes.getAbilityModifier(Ability::Dexterity);
    } else {
        modifier = _attributes.getAbilityModifier(Ability::Strength);
    }
    min += modifier;
    max += modifier;
}

void Creature::getOffhandDamage(int &min, int &max) const {
    getWeaponDamage(InventorySlot::leftWeapon, min, max);
}

void Creature::onEventSignalled(const std::string &name) {
    if (_footstepType == -1 || _walkmeshMaterial == -1 || name != "snd_footstep") {
        return;
    }
    std::shared_ptr<FootstepTypeSounds> sounds(_services.game.footstepSounds.get(_footstepType));
    if (!sounds) {
        return;
    }
    const Surface &surface = _services.game.surfaces.getSurface(_walkmeshMaterial);
    std::vector<std::shared_ptr<AudioClip>> materialSounds;
    if (surface.sound == "DT") {
        materialSounds = sounds->dirt;
    } else if (surface.sound == "GR") {
        materialSounds = sounds->grass;
    } else if (surface.sound == "ST") {
        materialSounds = sounds->stone;
    } else if (surface.sound == "WD") {
        materialSounds = sounds->wood;
    } else if (surface.sound == "WT") {
        materialSounds = sounds->water;
    } else if (surface.sound == "CP") {
        materialSounds = sounds->carpet;
    } else if (surface.sound == "MT") {
        materialSounds = sounds->metal;
    } else if (surface.sound == "LV") {
        materialSounds = sounds->leaves;
    }
    int index = randomInt(0, 3);
    if (index >= static_cast<int>(materialSounds.size())) {
        return;
    }
    std::shared_ptr<AudioClip> sound(materialSounds[index]);
    if (sound) {
        _audioSourceFootstep = _services.audio.player.play(sound, AudioType::Sound, false, 1.0f, true, _position);
    }
}

void Creature::giveGold(int amount) {
    _gold += amount;
}

void Creature::takeGold(int amount) {
    _gold -= amount;
}

bool Creature::navigateTo(const glm::vec3 &dest, bool run, float distance, float dt) {
    if (_movementRestricted)
        return false;

    float distToDest2 = getSquareDistanceTo(glm::vec2(dest));
    if (distToDest2 <= distance * distance) {
        setMovementType(Creature::MovementType::None);
        clearPath();
        return true;
    }

    bool updPath = true;
    if (_path) {
        uint32_t now = _services.system.clock.ticks();
        if (_path->destination == dest || now - _path->timeFound <= kKeepPathDuration) {
            advanceOnPath(run, dt);
            updPath = false;
        }
    }
    if (updPath) {
        updatePath(dest);
    }

    return false;
}

void Creature::advanceOnPath(bool run, float dt) {
    const glm::vec3 &origin = _position;
    size_t pointCount = _path->points.size();
    glm::vec3 dest;
    float distToDest;

    if (_path->pointIdx == pointCount) {
        dest = _path->destination;
        distToDest = glm::distance2(origin, dest);

    } else {
        const glm::vec3 &nextPoint = _path->points[_path->pointIdx];
        float distToNextPoint = glm::distance2(origin, nextPoint);
        float distToPathDest = glm::distance2(origin, _path->destination);

        if (distToPathDest < distToNextPoint) {
            dest = _path->destination;
            distToDest = distToPathDest;
            _path->pointIdx = static_cast<int>(pointCount);

        } else {
            dest = nextPoint;
            distToDest = distToNextPoint;
        }
    }

    if (distToDest <= 1.0f) {
        _path->selectNextPoint();
    } else {
        std::shared_ptr<Creature> creature(_game.getObjectById<Creature>(_id));
        if (_game.module()->area()->moveCreatureTowards(creature, dest, run, dt)) {
            setMovementType(run ? Creature::MovementType::Run : Creature::MovementType::Walk);
        } else {
            setMovementType(Creature::MovementType::None);
        }
    }
}

void Creature::updatePath(const glm::vec3 &dest) {
    std::vector<glm::vec3> points(_game.module()->area()->pathfinder().findPath(_position, dest));
    uint32_t now = _services.system.clock.ticks();
    setPath(dest, std::move(points), now);
}

std::string Creature::getAnimationName(AnimationType anim) const {
    std::string result;
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
        debug("CreatureAnimationResolver: unsupported animation type: " + std::to_string(static_cast<int>(anim)));
        return "";
    }
}

std::string Creature::getDieAnimation() const {
    return getFirstIfCreatureModel("cdie", "die");
}

std::string Creature::getFirstIfCreatureModel(std::string creatureAnim, std::string elseAnim) const {
    return _modelType == Creature::ModelType::Creature ? std::move(creatureAnim) : std::move(elseAnim);
}

std::string Creature::getDeadAnimation() const {
    return getFirstIfCreatureModel("cdead", "dead");
}

std::string Creature::getPauseAnimation() const {
    if (_modelType == Creature::ModelType::Creature)
        return "cpause1";

    // TODO: if (_lowHP) return "pauseinj"

    if (_combatState.active) {
        WeaponType type = WeaponType::None;
        WeaponWield wield = WeaponWield::None;
        getWeaponInfo(type, wield);

        int wieldNumber = getWeaponWieldNumber(wield);
        return str(boost::format("g%dr1") % wieldNumber);
    }

    return "pause1";
}

bool Creature::getWeaponInfo(WeaponType &type, WeaponWield &wield) const {
    std::shared_ptr<Item> item(getEquippedItem(InventorySlot::rightWeapon));
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

std::string Creature::getWalkAnimation() const {
    return getFirstIfCreatureModel("cwalk", "walk");
}

std::string Creature::getRunAnimation() const {
    if (_modelType == Creature::ModelType::Creature)
        return "crun";

    // TODO: if (_lowHP) return "runinj"

    if (_combatState.active) {
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

std::string Creature::getTalkNormalAnimation() const {
    return "tlknorm";
}

std::string Creature::getHeadTalkAnimation() const {
    return "talk";
}

static std::string formatCombatAnimation(const std::string &format, CreatureWieldType wield, int variant) {
    return str(boost::format(format) % static_cast<int>(wield) % variant);
}

std::string Creature::getAnimationName(CombatAnimation anim, CreatureWieldType wield, int variant) const {
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

std::string Creature::getActiveAnimationName() const {
    auto model = std::dynamic_pointer_cast<ModelSceneNode>(_sceneNode);
    if (!model)
        return "";

    return model->activeAnimationName();
}

std::shared_ptr<ModelSceneNode> Creature::buildModel() {
    std::string modelName(getBodyModelName());
    if (modelName.empty()) {
        return nullptr;
    }
    std::shared_ptr<Model> model(_services.resource.models.get(modelName));
    if (!model) {
        return nullptr;
    }
    auto &sceneGraph = _services.scene.graphs.get(_sceneName);
    auto sceneNode = sceneGraph.newModel(*model, ModelUsage::Creature);
    sceneNode->setCullable(true);
    sceneNode->setDrawDistance(_game.options().graphics.drawDistance);

    return sceneNode;
}

void Creature::finalizeModel(ModelSceneNode &body) {
    auto &sceneGraph = _services.scene.graphs.get(_sceneName);

    // Body texture

    if (!_envmap.empty()) {
        if (_envmap == "default") {
            body.setEnvironmentMap(&_services.graphics.textureRegistry.get(TextureName::defaultCubemapRgb));
        } else {
            body.setEnvironmentMap(_services.resource.textures.get(_envmap, TextureUsage::EnvironmentMap).get());
        }
    }
    std::string bodyTextureName(getBodyTextureName());
    if (!bodyTextureName.empty()) {
        std::shared_ptr<Texture> texture(_services.resource.textures.get(bodyTextureName, TextureUsage::Diffuse));
        if (texture) {
            body.setDiffuseMap(texture.get());
        }
    }

    // Mask

    std::shared_ptr<Model> maskModel;
    std::string maskModelName(getMaskModelName());
    if (!maskModelName.empty()) {
        maskModel = _services.resource.models.get(maskModelName);
    }

    // Head

    std::string headModelName(getHeadModelName());
    if (!headModelName.empty()) {
        std::shared_ptr<Model> headModel(_services.resource.models.get(headModelName));
        if (headModel) {
            std::shared_ptr<ModelSceneNode> headSceneNode(sceneGraph.newModel(*headModel, ModelUsage::Creature));
            body.attach(g_headHookNode, *headSceneNode);
            if (maskModel) {
                auto maskSceneNode = sceneGraph.newModel(*maskModel, ModelUsage::Equipment);
                headSceneNode->attach(g_maskHookNode, *maskSceneNode);
            }
        }
    }

    // Right weapon

    std::string rightWeaponModelName(getWeaponModelName(InventorySlot::rightWeapon));
    if (!rightWeaponModelName.empty()) {
        std::shared_ptr<Model> weaponModel(_services.resource.models.get(rightWeaponModelName));
        if (weaponModel) {
            std::shared_ptr<ModelSceneNode> weaponSceneNode(sceneGraph.newModel(*weaponModel, ModelUsage::Equipment));
            body.attach(g_rightHandNode, *weaponSceneNode);
        }
    }

    // Left weapon

    std::string leftWeaponModelName(getWeaponModelName(InventorySlot::leftWeapon));
    if (!leftWeaponModelName.empty()) {
        std::shared_ptr<Model> weaponModel(_services.resource.models.get(leftWeaponModelName));
        if (weaponModel) {
            std::shared_ptr<ModelSceneNode> weaponSceneNode(sceneGraph.newModel(*weaponModel, ModelUsage::Equipment));
            body.attach(g_leftHandNode, *weaponSceneNode);
        }
    }
}

std::string Creature::getBodyModelName() const {
    std::string column;

    if (_modelType == Creature::ModelType::Character) {
        column = "model";

        std::shared_ptr<Item> bodyItem(getEquippedItem(InventorySlot::body));
        if (bodyItem) {
            std::string baseBodyVar(bodyItem->baseBodyVariation());
            column += baseBodyVar;
        } else {
            column += "a";
        }

    } else {
        column = "race";
    }

    std::shared_ptr<TwoDa> appearance(_services.resource.twoDas.get("appearance"));
    if (!appearance) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }

    std::string modelName(appearance->getString(_appearance, column));
    boost::to_lower(modelName);

    return modelName;
}

std::string Creature::getBodyTextureName() const {
    std::string column;
    std::shared_ptr<Item> bodyItem(getEquippedItem(InventorySlot::body));

    if (_modelType == Creature::ModelType::Character) {
        column = "tex";

        if (bodyItem) {
            std::string baseBodyVar(bodyItem->baseBodyVariation());
            column += baseBodyVar;
        } else {
            column += "a";
        }
    } else {
        column = "racetex";
    }

    std::shared_ptr<TwoDa> appearance(_services.resource.twoDas.get("appearance"));
    if (!appearance) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }

    std::string texName(boost::to_lower_copy(appearance->getString(_appearance, column)));
    if (texName.empty())
        return "";

    if (_modelType == Creature::ModelType::Character) {
        bool texFound = false;
        if (bodyItem) {
            std::string tmp(str(boost::format("%s%02d") % texName % bodyItem->textureVariation()));
            std::shared_ptr<Texture> texture(_services.resource.textures.get(tmp, TextureUsage::Diffuse));
            if (texture) {
                texName = std::move(tmp);
                texFound = true;
            }
        }
        if (!texFound) {
            texName += "01";
        }
    }

    return texName;
}

std::string Creature::getHeadModelName() const {
    if (_modelType != Creature::ModelType::Character) {
        return "";
    }
    std::shared_ptr<TwoDa> appearance(_services.resource.twoDas.get("appearance"));
    if (!appearance) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    int headIdx = appearance->getInt(_appearance, "normalhead", -1);
    if (headIdx == -1) {
        return "";
    }
    std::shared_ptr<TwoDa> heads(_services.resource.twoDas.get("heads"));
    if (!heads) {
        throw ResourceNotFoundException("heads 2DA not found");
    }

    std::string modelName(heads->getString(headIdx, "head"));
    boost::to_lower(modelName);

    return modelName;
}

std::string Creature::getMaskModelName() const {
    std::shared_ptr<Item> headItem(getEquippedItem(InventorySlot::head));
    if (!headItem)
        return "";

    std::string modelName(boost::to_lower_copy(headItem->itemClass()));
    modelName += str(boost::format("_%03d") % headItem->modelVariation());

    return modelName;
}

std::string Creature::getWeaponModelName(int slot) const {
    std::shared_ptr<Item> bodyItem(getEquippedItem(slot));
    if (!bodyItem)
        return "";

    std::string modelName(bodyItem->itemClass());
    boost::to_lower(modelName);

    modelName += str(boost::format("_%03d") % bodyItem->modelVariation());

    return modelName;
}

void Creature::loadUTC(const resource::generated::UTC &utc) {
    _blueprintResRef = boost::to_lower_copy(utc.TemplateResRef);
    _race = static_cast<RacialType>(utc.Race);         // index into racialtypes.2da
    _subrace = static_cast<Subrace>(utc.SubraceIndex); // index into subrace.2da
    _appearance = utc.Appearance_Type;                 // index into appearance.2da
    _gender = static_cast<Gender>(utc.Gender);         // index into gender.2da
    _portraitId = utc.PortraitId;                      // index into portrait.2da
    _tag = boost::to_lower_copy(utc.Tag);
    _conversation = boost::to_lower_copy(utc.Conversation);
    _isPC = utc.IsPC;                               // always 0
    _faction = static_cast<Faction>(utc.FactionID); // index into repute.2da
    _disarmable = utc.Disarmable;
    _plot = utc.Plot;
    _interruptable = utc.Interruptable;
    _noPermDeath = utc.NoPermDeath;
    _notReorienting = utc.NotReorienting;
    _bodyVariation = utc.BodyVariation;
    _textureVar = utc.TextureVar;
    _minOneHP = utc.Min1HP;
    _partyInteract = utc.PartyInteract;
    _walkRate = utc.WalkRate; // index into creaturespeed.2da
    _naturalAC = utc.NaturalAC;
    _hitPoints = utc.HitPoints;
    _currentHitPoints = utc.CurrentHitPoints;
    _maxHitPoints = utc.MaxHitPoints;
    _forcePoints = utc.ForcePoints;
    _currentForce = utc.CurrentForce;
    _refBonus = utc.refbonus;
    _willBonus = utc.willbonus;
    _fortBonus = utc.fortbonus;
    _goodEvil = utc.GoodEvil;
    _challengeRating = utc.ChallengeRating;

    _onHeartbeat = boost::to_lower_copy(utc.ScriptHeartbeat);
    _onNotice = boost::to_lower_copy(utc.ScriptOnNotice);
    _onSpellAt = boost::to_lower_copy(utc.ScriptSpellAt);
    _onAttacked = boost::to_lower_copy(utc.ScriptAttacked);
    _onDamaged = boost::to_lower_copy(utc.ScriptDamaged);
    _onDisturbed = boost::to_lower_copy(utc.ScriptDisturbed);
    _onEndRound = boost::to_lower_copy(utc.ScriptEndRound);
    _onEndDialogue = boost::to_lower_copy(utc.ScriptEndDialogu);
    _onDialogue = boost::to_lower_copy(utc.ScriptDialogue);
    _onSpawn = boost::to_lower_copy(utc.ScriptSpawn);
    _onDeath = boost::to_lower_copy(utc.ScriptDeath);
    _onUserDefined = boost::to_lower_copy(utc.ScriptUserDefine);
    _onBlocked = boost::to_lower_copy(utc.ScriptOnBlocked);

    loadNameFromUTC(utc);
    loadSoundSetFromUTC(utc);
    loadBodyBagFromUTC(utc);
    loadAttributesFromUTC(utc);
    loadPerceptionRangeFromUTC(utc);

    for (auto &item : utc.Equip_ItemList) {
        equip(boost::to_lower_copy(item.EquippedRes));
    }
    for (auto &item : utc.ItemList) {
        std::string resRef(boost::to_lower_copy(item.InventoryRes));
        bool dropable = item.Dropable;
        addItem(resRef, 1, dropable);
    }

    // Unused fields:
    //
    // - Phenotype (not applicable, always 0)
    // - Description (not applicable)
    // - Subrace (unknown, we already use SubraceIndex)
    // - Deity (not applicable, always empty)
    // - LawfulChaotic (not applicable)
    // - ScriptRested (not applicable, mostly empty)
    // - PaletteID (toolset only)
    // - Comment (toolset only)
}

void Creature::loadNameFromUTC(const resource::generated::UTC &utc) {
    std::string firstName(_services.resource.strings.getText(utc.FirstName.first));
    std::string lastName(_services.resource.strings.getText(utc.LastName.first));
    if (!firstName.empty() && !lastName.empty()) {
        _name = firstName + " " + lastName;
    } else if (!firstName.empty()) {
        _name = firstName;
    }
}

void Creature::loadSoundSetFromUTC(const resource::generated::UTC &utc) {
    uint32_t soundSetIdx = utc.SoundSetFile;
    if (soundSetIdx == 0xffff) {
        return;
    }
    std::shared_ptr<TwoDa> soundSetTable(_services.resource.twoDas.get("soundset"));
    if (!soundSetTable) {
        return;
    }
    std::string soundSetResRef(soundSetTable->getString(soundSetIdx, "resref"));
    if (!soundSetResRef.empty()) {
        _soundSet = _services.resource.soundSets.get(soundSetResRef);
    }
}

void Creature::loadBodyBagFromUTC(const resource::generated::UTC &utc) {
    std::shared_ptr<TwoDa> bodyBags(_services.resource.twoDas.get("bodybag"));
    if (!bodyBags) {
        return;
    }
    int bodyBag = utc.BodyBag;
    _bodyBag.name = _services.resource.strings.getText(bodyBags->getInt(bodyBag, "name"));
    _bodyBag.appearance = bodyBags->getInt(bodyBag, "appearance");
    _bodyBag.corpse = bodyBags->getBool(bodyBag, "corpse");
}

void Creature::loadAttributesFromUTC(const resource::generated::UTC &utc) {
    CreatureAttributes &attributes = _attributes;
    attributes.setAbilityScore(Ability::Strength, utc.Str);
    attributes.setAbilityScore(Ability::Dexterity, utc.Dex);
    attributes.setAbilityScore(Ability::Constitution, utc.Con);
    attributes.setAbilityScore(Ability::Intelligence, utc.Int);
    attributes.setAbilityScore(Ability::Wisdom, utc.Wis);
    attributes.setAbilityScore(Ability::Charisma, utc.Cha);

    for (auto &classStrct : utc.ClassList) {
        auto clazz = static_cast<ClassType>(classStrct.Class);
        int level = classStrct.ClassLevel;
        attributes.addClassLevels(_services.game.classes.get(clazz).get(), level);
        for (auto &spellStrct : classStrct.KnownList0) {
            auto spell = static_cast<SpellType>(spellStrct.Spell);
            attributes.addSpell(spell);
        }
    }

    for (size_t i = 0; i < utc.SkillList.size(); ++i) {
        SkillType skill = static_cast<SkillType>(i);
        attributes.setSkillRank(skill, utc.SkillList[i].Rank);
    }

    for (auto &featStrct : utc.FeatList) {
        auto feat = static_cast<FeatType>(featStrct.Feat);
        _attributes.addFeat(feat);
    }
}

void Creature::loadPerceptionRangeFromUTC(const resource::generated::UTC &utc) {
    std::shared_ptr<TwoDa> ranges(_services.resource.twoDas.get("ranges"));
    if (!ranges) {
        return;
    }
    int rangeIdx = utc.PerceptionRange;
    _perception.sightRange = ranges->getFloat(rangeIdx, "primaryrange");
    _perception.hearingRange = ranges->getFloat(rangeIdx, "secondaryrange");
}

} // namespace game

} // namespace reone
