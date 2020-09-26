/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../core/log.h"
#include "../../core/streamutil.h"
#include "../../net/types.h"
#include "../../resources/resources.h"
#include "../../script/types.h"

#include "../blueprint/blueprints.h"
#include "../object/factory.h"
#include "../script/util.h"

using namespace std;

using namespace reone::net;
using namespace reone::render;
using namespace reone::resources;
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

Creature::Action::Action(ActionType type) : type(type) {
}

Creature::Action::Action(ActionType type, const shared_ptr<Object> &object, float distance) : type(type), object(object), distance(distance) {
}

Creature::Action::Action(ActionType type, const ExecutionContext &ctx) : type(type), context(ctx) {
}

Creature::Creature(uint32_t id, ObjectFactory *objectFactory) :
    SpatialObject(id, ObjectType::Creature), _objectFactory(objectFactory) {

    assert(_objectFactory);
    _drawDistance = 2048.0f;
    _fadeDistance = 0.25f * _drawDistance;
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

    loadBlueprint(templResRef);
    updateTransform();
}

void Creature::loadBlueprint(const string &resRef) {
    _blueprint = Blueprints.findCreature(resRef);
    _tag = _blueprint->tag();

    for (auto &item : _blueprint->equipment()) {
        equip(item);
    }
    shared_ptr<TwoDaTable> appearanceTable(Resources.find2DA("appearance"));
    loadAppearance(*appearanceTable, _blueprint->appearance());
}

void Creature::loadAppearance(const TwoDaTable &table, int row) {
    _config.appearance = row;
    _modelType = parseModelType(table.getString(row, "modeltype"));
    _walkSpeed = table.getFloat(row, "walkdist", 0.0f);
    _runSpeed = table.getFloat(row, "rundist", 0.0f);

    switch (_modelType) {
        case ModelType::Character:
            loadCharacterAppearance(table, row);
            break;
        default:
            loadDefaultAppearance(table, row);
            break;
    }

    if (_model) {
        _model->setDefaultAnimation(getPauseAnimation());
        _model->playDefaultAnimation();
    }
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

void Creature::loadCharacterAppearance(const TwoDaTable &table, int row) {
    ResourceManager &resources = Resources;

    string modelColumn("model");
    string texColumn("tex");
    bool bodyEquipped = false;

    auto it = _equipment.find(kInventorySlotBody);
    if (it != _equipment.end()) {
        modelColumn += it->second->getTemplate().baseBodyVariation();
        texColumn += it->second->getTemplate().baseBodyVariation();
        bodyEquipped = true;
    } else {
        modelColumn += "a";
        texColumn += "a";
    }

    const string &modelName = table.getString(row, modelColumn);
    _model = make_unique<ModelSceneNode>(resources.findModel(modelName));
    _model->setLightingEnabled(true);

    string texName(table.getString(row, texColumn));
    if (bodyEquipped) {
        texName += str(boost::format("%02d") % it->second->getTemplate().textureVariation());
    } else {
        texName += "01";
    }
    _model->changeTexture(texName);

    it = _equipment.find(kInventorySlotRightWeapon);
    if (it != _equipment.end()) {
        string weaponModelName(it->second->getTemplate().itemClass());
        weaponModelName += str(boost::format("_%03d") % it->second->getTemplate().modelVariation());
        _model->attach("rhand", resources.findModel(weaponModelName));
    }

    int headIdx = table.getInt(row, "normalhead", -1);
    if (headIdx == -1) return;

    shared_ptr<TwoDaTable> headTable(resources.find2DA("heads"));
    loadHead(*headTable, headIdx);
}

void Creature::loadHead(const TwoDaTable &table, int row) {
    ResourceManager &resources = ResourceManager::instance();
    const string &modelName = table.getString(row, "head");
    _model->attach(g_headHookNode, resources.findModel(modelName));
}

void Creature::loadDefaultAppearance(const TwoDaTable &table, int row) {
    ResourceManager &resources = ResourceManager::instance();

    const string &modelName = table.getString(row, "race");
    _model = make_unique<ModelSceneNode>(resources.findModel(modelName));
    _model->setLightingEnabled(true);

    const string &raceTexName = table.getString(row, "racetex");
    if (!raceTexName.empty()) {
        _model->changeTexture(boost::to_lower_copy(raceTexName));
    }

    auto it = _equipment.find(kInventorySlotRightWeapon);
    if (it != _equipment.end()) {
        string weaponModelName(it->second->getTemplate().itemClass());
        weaponModelName += str(boost::format("_%03d") % it->second->getTemplate().modelVariation());
        _model->attach("rhand", resources.findModel(weaponModelName));
    }
}

void Creature::load(const CreatureConfiguration &config) {
    for (auto &item : config.equipment) {
        equip(item);
    }
    shared_ptr<TwoDaTable> appearanceTable(Resources.find2DA("appearance"));
    loadAppearance(*appearanceTable, config.appearance);
    loadPortrait(config.appearance);
}

void Creature::loadPortrait(int appearance) {
    shared_ptr<TwoDaTable> portraits(Resources.find2DA("portraits"));
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

    _portrait = Resources.findTexture(resRef, TextureType::GUI);
}

void Creature::playDefaultAnimation() {
    if (_model) _model->playDefaultAnimation();
}

void Creature::playGreetingAnimation() {
    if (_movementType != MovementType::None) return;

    animate(g_animGreeting, kAnimationPropagate);
}

void Creature::playTalkAnimation() {
    animate(g_animTalkBody, kAnimationLoop);
    animate(g_headHookNode, g_animTalkHead, kAnimationLoop, 0.25f);
}

void Creature::clearActions() {
    _actions.clear();
}

void Creature::enqueueAction(Action action) {
    _actions.emplace_back(std::move(action));
}

void Creature::popCurrentAction() {
    assert(!_actions.empty());
    _actions.pop_front();
}

void Creature::equip(const string &resRef) {
    shared_ptr<ItemBlueprint> itemTempl(Blueprints.findItem(resRef));

    shared_ptr<Item> item(_objectFactory->newItem());
    item->load(itemTempl.get());

    switch (item->getTemplate().type()) {
        case ItemType::Armor:
            _equipment[kInventorySlotBody] = move(item);
            break;
        case ItemType::RightWeapon:
            _equipment[kInventorySlotRightWeapon] = move(item);
            break;
        default:
            break;
    }
}

void Creature::saveTo(AreaState &state) const {
    if (_tag.empty()) return;

    CreatureState crState;
    crState.position = _position;
    crState.heading = _heading;

    state.creatures[_tag] = move(crState);
}

void Creature::loadState(const AreaState &state) {
    if (_tag.empty()) return;

    auto it = state.creatures.find(_tag);
    if (it == state.creatures.end()) return;

    const CreatureState &crState = it->second;

    _position = crState.position;
    _heading = crState.heading;

    updateTransform();
}

void Creature::setTag(const string &tag) {
    _tag = tag;
}

void Creature::setMovementType(MovementType type) {
    if (!_model || _movementType == type) return;

    switch (type) {
        case MovementType::Walk:
            _model->animate(getWalkAnimation(), kAnimationLoop | kAnimationPropagate);
            break;
        case MovementType::Run:
            _model->animate(getRunAnimation(), kAnimationLoop | kAnimationPropagate);
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

const string &Creature::getPauseAnimation() {
    switch (_modelType) {
        case ModelType::Creature:
            return g_animPauseCreature;
        default:
            return g_animPauseCharacter;
    }
}

const string &Creature::getWalkAnimation() {
    switch (_modelType) {
        case ModelType::Creature:
            return g_animWalkCreature;
        default:
            return g_animWalkCharacter;
    }
}

const string &Creature::getRunAnimation() {
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

bool Creature::hasActions() const {
    return !_actions.empty();
}

const Creature::Action &Creature::currentAction() const {
    assert(!_actions.empty());
    return _actions.front();
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

} // namespace game

} // namespace reone
