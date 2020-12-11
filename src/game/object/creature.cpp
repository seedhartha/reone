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

#include "creature.h"

#include <climits>

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../common/timer.h"
#include "../../net/types.h"
#include "../../render/models.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../script/types.h"

#include "../action/attack.h"
#include "../blueprint/blueprints.h"
#include "../rp/classutil.h"
#include "../portraitutil.h"
#include "../script/util.h"

#include "objectfactory.h"

using namespace std;

using namespace reone::net;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

constexpr int kStrRefRemains = 38151;

static string g_headHookNode("headhook");
static string g_talkDummyNode("talkdummy");

Creature::Creature(uint32_t id, ObjectFactory *objectFactory, SceneGraph *sceneGraph) :
    SpatialObject(id, ObjectType::Creature, objectFactory, sceneGraph),
    _modelBuilder(this),
    _animResolver(this) {

    _drawDistance = 2048.0f;
}

bool Creature::isSelectable() const {
    return !_dead || !_items.empty();
}

void Creature::load(const GffStruct &gffs) {
    loadTransform(gffs);
    loadBlueprint(gffs);
}

void Creature::loadTransform(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float dirX = gffs.getFloat("XOrientation");
    float dirY = gffs.getFloat("YOrientation");
    _heading = -glm::atan(dirX, dirY);

    updateTransform();
}

void Creature::loadBlueprint(const GffStruct &gffs) {
    string resRef(boost::to_lower_copy(gffs.getString("TemplateResRef")));
    shared_ptr<CreatureBlueprint> blueprint(Blueprints::instance().getCreature(resRef));
    load(blueprint);
}

void Creature::load(const shared_ptr<CreatureBlueprint> &blueprint) {
    _blueprintResRef = blueprint->resRef();

    blueprint->load(*this);

    shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));
    loadAppearance(*appearance, _appearance);
}

void Creature::loadAppearance(const TwoDaTable &table, int row) {
    _appearance = row;
    _config.appearance = row;
    _modelType = parseModelType(table.getString(row, "modeltype"));
    _walkSpeed = table.getFloat(row, "walkdist", 0.0f);
    _runSpeed = table.getFloat(row, "rundist", 0.0f);

    updateModel();
}

Creature::ModelType Creature::parseModelType(const string &s) const {
    if (s == "S" || s == "L") {
        return ModelType::Creature;
    } else if (s == "F") {
        return ModelType::Droid;
    } else if (s == "B") {
        return ModelType::Character;
    }

    throw logic_error("Unsupported model type: " + s);
}

void Creature::updateModel() {
    if (_model) {
        _sceneGraph->removeRoot(_model);
    }
    _model = _modelBuilder.build();

    if (_model) {
        _headModel = _model->getAttachedModel(g_headHookNode);
        _model->setLocalTransform(_transform);
        _sceneGraph->addRoot(_model);
        _animDirty = true;
    }
}

void Creature::load(const CreatureConfiguration &config) {
    if (config.blueprint) {
        load(config.blueprint);
    } else {
        shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));
        loadAppearance(*appearance, config.appearance);
        loadPortrait(config.appearance);
        _attributes.addClassLevels(config.clazz, 1);
        _currentHitPoints = _hitPoints = _maxHitPoints = getClassHitPoints(config.clazz, 1);
    }
    for (auto &item : config.equipment) {
        equip(item);
    }
}

void Creature::loadPortrait(int appearance) {
    shared_ptr<TwoDaTable> portraits(Resources::instance().get2DA("portraits"));
    string appearanceString(to_string(appearance));

    const TwoDaRow *row = portraits->findRow([&appearanceString](const TwoDaRow &r) {
        return
            r.getString("appearancenumber") == appearanceString ||
            r.getString("appearance_s") == appearanceString ||
            r.getString("appearance_l") == appearanceString;
    });
    if (!row) {
        warn("Creature: portrait not found: " + appearanceString);
        return;
    }
    string resRef(row->getString("baseresref"));
    boost::to_lower(resRef);

    _portrait = Textures::instance().get(resRef, TextureType::GUI);
}

void Creature::update(float dt) {
    SpatialObject::update(dt);
    updateModelAnimation();
    updateHealth();
}

void Creature::updateModelAnimation() {
    if (!_model) return;

    if (_animFireForget) {
        if (!_model->isAnimationFinished()) return;

        _animFireForget = false;
        _animDirty = true;
    }
    if (!_animDirty) return;

    switch (_movementType) {
        case MovementType::Run:
            _model->playAnimation(_animResolver.getRunAnimation(), kAnimationLoop | kAnimationPropagate | kAnimationBlend);
            break;
        case MovementType::Walk:
            _model->playAnimation(_animResolver.getWalkAnimation(), kAnimationLoop | kAnimationPropagate | kAnimationBlend);
            break;
        default:
            if (_dead) {
                _model->playAnimation(_animResolver.getDeadAnimation(), kAnimationPropagate | kAnimationBlend);
            } else if (_talking) {
                _model->playAnimation(_animResolver.getTalkAnimation(), kAnimationLoop | kAnimationPropagate);
                if (_headModel) {
                    _headModel->playAnimation(_animResolver.getHeadTalkAnimation(), kAnimationLoop | kAnimationOverlay, 0.25f);
                }
            } else {
                _model->playAnimation(_animResolver.getPauseAnimation(), kAnimationLoop | kAnimationPropagate | kAnimationBlend);
            }
            break;
    }

    _animDirty = false;
}

void Creature::updateHealth() {
    if (_dead || _currentHitPoints > 0) return;

    playAnimation(Animation::Die);
    _dead = true;
    _title = Resources::instance().getString(kStrRefRemains);

    debug(boost::format("Creature: '%s' is dead") % _tag, 2);
}

void Creature::clearAllActions() {
    SpatialObject::clearAllActions();
    setMovementType(MovementType::None);
}

void Creature::playAnimation(Animation anim) {
    if (!_model || _movementType != MovementType::None) return;

    string animName;
    switch (anim) {
        case Animation::UnlockDoor:
            animName = _animResolver.getUnlockDoorAnimation();
            break;
        case Animation::DuelAttack:
            animName = _animResolver.getDuelAttackAnimation();
            break;
        case Animation::BashAttack:
            animName = _animResolver.getBashAttackAnimation();
            break;
        case Animation::Dodge:
            animName = _animResolver.getDodgeAnimation();
            break;
        case Animation::Knockdown:
            animName = _animResolver.getKnockdownAnimation();
            break;
        case Animation::Die:
            animName = _animResolver.getDieAnimation();
            break;
        default:
            break;
    }
    if (!animName.empty()) {
        _model->playAnimation(animName, kAnimationPropagate | kAnimationBlend);
        _animFireForget = true;
    }
}

void Creature::equip(const string &resRef) {
    shared_ptr<ItemBlueprint> blueprint(Blueprints::instance().getItem(resRef));

    shared_ptr<Item> item(_objectFactory->newItem());
    item->load(blueprint);

    if (item->isEquippable(kInventorySlotBody)) {
        equip(kInventorySlotBody, item);
    } else if (item->isEquippable(kInventorySlotRightWeapon)) {
        equip(kInventorySlotRightWeapon, item);
    }
}

void Creature::equip(InventorySlot slot, const shared_ptr<Item> &item) {
    if (item->isEquippable(slot)) {
        _equipment[slot] = item;
    }
    if (_model) {
        updateModel();
    }
}

void Creature::unequip(const shared_ptr<Item> &item) {
    for (auto &equipped : _equipment) {
        if (equipped.second == item) {
            _equipment.erase(equipped.first);
            break;
        }
    }
    if (_model) {
        updateModel();
    }
}

shared_ptr<Item> Creature::getEquippedItem(InventorySlot slot) const {
    auto equipped = _equipment.find(slot);
    return equipped != _equipment.end() ? equipped->second : nullptr;
}

bool Creature::isSlotEquipped(InventorySlot slot) const {
    return _equipment.find(slot) != _equipment.end();
}

void Creature::setMovementType(MovementType type) {
    if (_movementType == type) return;

    _movementType = type;
    _animDirty = true;
    _animFireForget = false;
}

void Creature::setTalking(bool talking) {
    if (_talking == talking) return;

    _talking = talking;
    _animDirty = true;
}

void Creature::setPath(const glm::vec3 &dest, vector<glm::vec3> &&points, uint32_t timeFound) {
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
    unique_ptr<Path> path(new Path());
    path->destination = dest;
    path->points = points;
    path->timeFound = timeFound;
    path->pointIdx = pointIdx;

    _path = move(path);
}

void Creature::clearPath() {
    _path.reset();
}

const string &Creature::blueprintResRef() const {
    return _blueprintResRef;
}

Gender Creature::gender() const {
    return _config.gender;
}

Creature::ModelType Creature::modelType() const {
    return _modelType;
}

int Creature::appearance() const {
    return _config.appearance;
}

shared_ptr<Texture> Creature::portrait() const {
    return _portrait;
}

const map<InventorySlot, shared_ptr<Item>> &Creature::equipment() const {
    return _equipment;
}

shared_ptr<Creature::Path> &Creature::path() {
    return _path;
}

float Creature::walkSpeed() const {
    return _walkSpeed;
}

float Creature::runSpeed() const {
    return _runSpeed;
}

CreatureAttributes &Creature::attributes() {
    return _attributes;
}

glm::vec3 Creature::getSelectablePosition() const {
    glm::vec3 position;

    if (_model->getNodeAbsolutePosition(g_talkDummyNode, position)) {
        return _model->absoluteTransform() * glm::vec4(position, 1.0f);
    }

    return _model->getCenterOfAABB();
}

Faction Creature::faction() const {
    return _faction;
}

float Creature::getAttackRange() const {
    float result = kDefaultAttackRange;

    shared_ptr<Item> item(getEquippedItem(kInventorySlotRightWeapon));
    if (item && item->attackRange() > kDefaultAttackRange) {
        result = item->attackRange();
    }

    return result;
}

void Creature::setFaction(Faction faction) {
    _faction = faction;
}

bool Creature::isMovementRestricted() const {
    return _movementRestricted;
}

bool Creature::isInCombat() const {
    return _inCombat;
}

void Creature::setMovementRestricted(bool restricted) {
    _movementRestricted = restricted;
}

void Creature::setInCombat(bool active) {
    _inCombat = active;
}

void Creature::setOnSpawn(const string &onSpawn) {
    _onSpawn = onSpawn;
}

void Creature::setOnUserDefined(const string &onUserDefined) {
    _onUserDefined = onUserDefined;
}

} // namespace game

} // namespace reone
