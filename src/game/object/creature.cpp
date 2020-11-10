/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../net/types.h"
#include "../../render/models.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../script/types.h"
#include "../../common/log.h"
#include "../../common/streamutil.h"

#include "../blueprint/blueprints.h"
#include "../portraits.h"
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

static string g_animPauseCreature("cpause1");
static string g_animPauseCharacter("pause1");
static string g_animWalkCreature("cwalk");
static string g_animWalkCharacter("walk");
static string g_animRunCreature("crun");
static string g_animRunCharacter("run");
static string g_animTalkHead("talk");
static string g_animTalkBody("tlknorm");
static string g_animGreeting("greeting");

static string g_headHookNode("headhook");

Creature::Creature(uint32_t id, ObjectFactory *objectFactory, SceneGraph *sceneGraph) :
    SpatialObject(id, ObjectType::Creature, sceneGraph), _objectFactory(objectFactory) {

    _drawDistance = 2048.0f;
    _fadeDistance = 0.25f * _drawDistance;
    _selectable = true;
}

void Creature::load(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float dirX = gffs.getFloat("XOrientation");
    float dirY = gffs.getFloat("YOrientation");
    _heading = -glm::atan(dirX, dirY);

    string templResRef(gffs.getString("TemplateResRef"));
    boost::to_lower(templResRef);

    shared_ptr<CreatureBlueprint> blueprint(Blueprints::instance().getCreature(templResRef));
    load(blueprint);

    updateTransform();
}

void Creature::load(const shared_ptr<CreatureBlueprint> &blueprint) {
    _blueprint = blueprint;
    _tag = _blueprint->tag();

    for (auto &item : _blueprint->equipment()) {
        equip(item);
    }
    shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));
    loadAppearance(*appearance, _blueprint->appearance());

    string portrait(getPortrait(_blueprint->portraitId()));
    _portrait = Textures::instance().get(portrait, TextureType::GUI);

    string firstName(_blueprint->firstName());
    string lastName(_blueprint->lastName());
    if (!firstName.empty() && !lastName.empty()) {
        _title = firstName + " " + lastName;
    } else if (!firstName.empty()) {
        _title = firstName;
    }

    _attributes = blueprint->attributes();
    _onSpawn = blueprint->onSpawn();
    _onUserDefined = blueprint->onUserDefined();
}

void Creature::loadAppearance(const TwoDaTable &table, int row) {
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
    string bodyModelName(getBodyModelName());
    string bodyTextureName(getBodyTextureName());
    string headModelName(getHeadModelName());
    string leftWeaponModelName(getWeaponModelName(kInventorySlotLeftWeapon));
    string rightWeaponModelName(getWeaponModelName(kInventorySlotRightWeapon));

    // Body

    if (!_model) {
        _model = make_unique<ModelSceneNode>(_sceneGraph, Models::instance().get(bodyModelName));
        _model->setLightingEnabled(true);
    } else {
        _model->setModel(Models::instance().get(bodyModelName));
    }

    // Body texture

    shared_ptr<Texture> texture;
    if (!bodyTextureName.empty()) {
        texture = Textures::instance().get(bodyTextureName, TextureType::Diffuse);
    }
    _model->setTextureOverride(texture);

    // Head

    shared_ptr<Model> headModel;
    if (!headModelName.empty()) {
        headModel = Models::instance().get(headModelName);
    }
    _headModel = _model->attach(g_headHookNode, headModel);

    // Right weapon

    shared_ptr<Model> leftWeaponModel;
    if (!leftWeaponModelName.empty()) {
        leftWeaponModel = Models::instance().get(leftWeaponModelName);
    }
    _model->attach("lhand", leftWeaponModel);

    // Right weapon

    shared_ptr<Model> rightWeaponModel;
    if (!rightWeaponModelName.empty()) {
        rightWeaponModel = Models::instance().get(rightWeaponModelName);
    }
    _model->attach("rhand", rightWeaponModel);

    // Animation

    _model->setDefaultAnimation(getPauseAnimation());
    _model->playDefaultAnimation();
}

string Creature::getBodyModelName() const {
    string column;

    if (_modelType == ModelType::Character) {
        column = "model";

        shared_ptr<Item> bodyItem(getEquippedItem(kInventorySlotBody));
        if (bodyItem) {
            string baseBodyVar(bodyItem->blueprint().baseBodyVariation());
            column += baseBodyVar;
        } else {
            column += "a";
        }

    } else {
        column = "race";
    }

    shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));

    string modelName(appearance->getString(_config.appearance, column));
    boost::to_lower(modelName);

    return move(modelName);
}

string Creature::getBodyTextureName() const {
    string column;
    shared_ptr<Item> bodyItem(getEquippedItem(kInventorySlotBody));

    if (_modelType == ModelType::Character) {
        column = "tex";

        if (bodyItem) {
            string baseBodyVar(bodyItem->blueprint().baseBodyVariation());
            column += baseBodyVar;
        } else {
            column += "a";
        }
    } else {
        column = "racetex";
    }

    shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));

    string texName(appearance->getString(_config.appearance, column));
    boost::to_lower(texName);

    if (bodyItem) {
        texName += str(boost::format("%02d") % bodyItem->blueprint().textureVariation());
    } else {
        texName += "01";
    }

    return move(texName);
}

string Creature::getHeadModelName() const {
    if (_modelType != ModelType::Character) return "";

    shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));

    int headIdx = appearance->getInt(_config.appearance, "normalhead", -1);
    if (headIdx == -1) return "";

    shared_ptr<TwoDaTable> heads(Resources::instance().get2DA("heads"));

    string modelName(heads->getString(headIdx, "head"));
    boost::to_lower(modelName);

    return move(modelName);
}

string Creature::getWeaponModelName(InventorySlot slot) const {
    shared_ptr<Item> bodyItem(getEquippedItem(slot));
    if (!bodyItem) return "";

    string modelName(bodyItem->blueprint().itemClass());
    boost::to_lower(modelName);

    modelName += str(boost::format("_%03d") % bodyItem->blueprint().modelVariation());

    return move(modelName);
}

void Creature::load(const CreatureConfiguration &config) {
    if (config.blueprint) {
        load(config.blueprint);
    } else {
        shared_ptr<TwoDaTable> appearance(Resources::instance().get2DA("appearance"));
        loadAppearance(*appearance, config.appearance);
        loadPortrait(config.appearance);
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

void Creature::playDefaultAnimation() {
    if (_model) {
        _model->playDefaultAnimation();
    }
}

void Creature::playGreetingAnimation() {
    if (_movementType != MovementType::None) return;

    playAnimation(g_animGreeting, kAnimationPropagate);
}

void Creature::playTalkAnimation() {
    if (!_model) return;

    _model->playAnimation(g_animTalkBody, kAnimationLoop | kAnimationPropagate);

    if (_headModel) {
        _headModel->playAnimation(g_animTalkHead, kAnimationLoop | kAnimationOverlay, 0.25f);
    }
}

void Creature::equip(const string &resRef) {
    shared_ptr<ItemBlueprint> blueprint(Blueprints::instance().getItem(resRef));

    shared_ptr<Item> item(_objectFactory->newItem());
    item->load(blueprint);

    if (blueprint->isEquippable(kInventorySlotBody)) {
        equip(kInventorySlotBody, item);
    } else if (blueprint->isEquippable(kInventorySlotRightWeapon)) {
        equip(kInventorySlotRightWeapon, item);
    }

    _items.push_back(item);
}

void Creature::equip(InventorySlot slot, const shared_ptr<Item> &item) {
    const ItemBlueprint &blueprint = item->blueprint();

    if (blueprint.isEquippable(slot)) {
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

void Creature::setTag(const string &tag) {
    _tag = tag;
}

void Creature::setMovementType(MovementType type) {
    if (!_model || _movementType == type) return;

    switch (type) {
        case MovementType::Walk:
            _model->playAnimation(getWalkAnimation(), kAnimationLoop | kAnimationPropagate | kAnimationBlend);
            break;
        case MovementType::Run:
            _model->playAnimation(getRunAnimation(), kAnimationLoop | kAnimationPropagate | kAnimationBlend);
            break;
        default:
            if (_talking) {
                playTalkAnimation();
            } else {
                _model->playDefaultAnimation();
            }
            break;
    }

    _movementType = type;
}

void Creature::setTalking(bool talking) {
    if (_talking == talking) return;

    if (_movementType == MovementType::None) {
        if (talking) {
            playTalkAnimation();
        } else {
            playDefaultAnimation();
        }
    }
    _talking = talking;
}

const string &Creature::getPauseAnimation() const {
    switch (_modelType) {
        case ModelType::Creature:
            return g_animPauseCreature;
        default:
            return g_animPauseCharacter;
    }
}

const string &Creature::getWalkAnimation() const {
    switch (_modelType) {
        case ModelType::Creature:
            return g_animWalkCreature;
        default:
            return g_animWalkCharacter;
    }
}

const string &Creature::getRunAnimation() const {
    switch (_modelType) {
        case ModelType::Creature:
            return g_animRunCreature;
        default:
            return g_animRunCharacter;
    }
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

Gender Creature::gender() const {
    return _config.gender;
}

int Creature::getClassLevel(ClassType clazz) const {
    return _config.clazz == clazz ? 1 : 0;
}

int Creature::appearance() const {
    return _config.appearance;
}

shared_ptr<Texture> Creature::portrait() const {
    return _portrait;
}

string Creature::conversation() const {
    return _blueprint ? _blueprint->conversation() : "";
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

glm::vec3 Creature::selectablePosition() const {
    glm::vec3 position;

    if (_model->getNodeAbsolutePosition(g_headHookNode, position)) {
        return _model->absoluteTransform() * glm::vec4(position, 1.0f);
    }

    return _model->getCenterOfAABB();
}

} // namespace game

} // namespace reone
