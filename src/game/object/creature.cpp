/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../../common/exception/validation.h"
#include "../../graphics/models.h"
#include "../../graphics/services.h"
#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"
#include "../../resource/strings.h"
#include "../../scene/collision.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"

#include "../astar.h"
#include "../gameinterface.h"
#include "../services.h"

#include "factory.h"
#include "item.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kCreatureRunSpeed = 3.96f;
static constexpr float kPlottedPathDuration = 1.0f; // seconds

static const string kHeadHookNodeName = "headhook";

void Creature::loadFromGit(const Gff &git) {
    auto xPosition = git.getFloat("XPosition");
    auto yPosition = git.getFloat("YPosition");
    auto zPosition = git.getFloat("ZPosition");
    auto xOrientation = git.getFloat("XOrientation");
    auto yOrientation = git.getFloat("YOrientation");
    auto templateResRef = git.getString("TemplateResRef");

    _position = glm::vec3(xPosition, yPosition, zPosition);
    _facing = -glm::atan(xOrientation, yOrientation);

    loadFromUtc(templateResRef);
}

void Creature::loadFromUtc(const string &templateResRef) {
    // From UTC

    auto utc = _resourceSvc.gffs.get(templateResRef, ResourceType::Utc);
    if (!utc) {
        throw ValidationException("UTC not found: " + templateResRef);
    }
    auto tag = utc->getString("Tag");
    auto firstName = _resourceSvc.strings.get(utc->getInt("FirstName"));
    auto appearanceType = utc->getInt("Appearance_Type");
    auto conversation = utc->getString("Conversation");

    auto itemList = utc->getList("ItemList");
    for (auto &utcItem : itemList) {
        auto inventoryRes = utcItem->getString("InventoryRes");
        auto item = static_pointer_cast<Item>(_objectFactory.newItem());
        item->loadFromUti(inventoryRes);
        _items.push_back(item.get());
    }

    auto equipItemList = utc->getList("Equip_ItemList");
    for (auto &utcItem : equipItemList) {
        auto equippedRes = utcItem->getString("EquippedRes");
        auto item = static_pointer_cast<Item>(_objectFactory.newItem());
        item->loadFromUti(equippedRes);
        // TODO: equip
        // _equipment[InventorySlot::rightWeapon] = item.get();
    }

    // From appearance 2DA

    auto appearanceTable = _resourceSvc.twoDas.get("appearance");
    if (!appearanceTable) {
        throw ValidationException("appearance 2DA not found");
    }
    auto modelType = appearanceTable->getString(appearanceType, "modeltype");
    auto race = appearanceTable->getString(appearanceType, "race");

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;
    auto model = _graphicsSvc.models.get(race);
    if (model) {
        sceneNode = _sceneGraph->newModel(*model, ModelUsage::Creature);
        sceneNode->init();
        if (modelType == "B") {
            auto normalHead = appearanceTable->getInt(appearanceType, "normalhead");
            auto backupHead = appearanceTable->getInt(appearanceType, "backuphead");
            auto headsTable = _resourceSvc.twoDas.get("heads");
            if (!headsTable) {
                throw ValidationException("heads 2DA not found");
            }
            auto head = headsTable->getString(normalHead, "head");
            auto headModel = _graphicsSvc.models.get(head);
            if (headModel) {
                auto headSceneNode = _sceneGraph->newModel(*headModel, ModelUsage::Creature);
                headSceneNode->init();
                sceneNode->attach(kHeadHookNodeName, *headSceneNode);
            }
        }
        sceneNode->setUser(*this);
        sceneNode->setCullable(true);
        sceneNode->setPickable(true);
    }

    //

    _tag = move(tag);
    _name = move(firstName);
    _conversation = move(conversation);
    _modelType = static_cast<ModelType>(modelType[0]);
    _sceneNode = sceneNode.get();

    flushTransform();
}

bool Creature::moveTo(Object &other, bool run, float range, float delta) {
    return moveTo(glm::vec2(other.position()), run, range, delta);
}

bool Creature::moveTo(const glm::vec2 &destination, bool run, float range, float delta) {
    auto dist = square2dDistanceTo(destination);
    if (dist < range * range) {
        setState(Creature::State::Pause);
        return true;
    }

    auto point = destination;

    bool sameDestination = _plottedPath.destination == destination;
    if (sameDestination || _plottedPath.duration > 0.0f) {
        do {
            point = _plottedPath.points.front();
            if (square2dDistanceTo(point) < 1.0f) {
                _plottedPath.points.pop();
            } else {
                break;
            }
        } while (!_plottedPath.points.empty());
        if (sameDestination) {
            _plottedPath.duration = kPlottedPathDuration;
        } else {
            _plottedPath.duration = glm::max(0.0f, _plottedPath.duration - delta);
        }
    } else {
        auto start = glm::vec2(_position);
        if (!_plottedPath.points.empty()) {
            start = _plottedPath.points.front();
        }
        auto aStarPoints = _gameSvc.aStar.plotPath(start, destination);
        auto points = queue<glm::vec2>();
        for (auto &point : aStarPoints) {
            points.push(point);
        }
        _plottedPath.destination = destination;
        _plottedPath.points.swap(points);
        _plottedPath.duration = kPlottedPathDuration;
    }

    face(point);
    setState(run ? Creature::State::Run : Creature::State::Walk);
    moveForward(delta);

    return false;
}

bool Creature::moveForward(float delta) {
    auto dir = glm::vec2(-glm::sin(_facing), glm::cos(_facing));
    auto newPosition = glm::vec2(_position) + delta * kCreatureRunSpeed * dir;

    Collision collision;
    if (!_sceneGraph->testElevation(glm::vec2(newPosition), collision)) {
        return false;
    }

    _position = glm::vec3(newPosition, collision.intersection.z);
    flushTransform();
    return true;
}

void Creature::update(float delta) {
    Object::update(delta);

    if (_sceneNode) {
        string animName;
        if (_state == State::Pause) {
            animName = "pause1";
        } else if (_state == State::Walk) {
            animName = "walk";
        } else if (_state == State::Run) {
            animName = "run";
        }
        if (_modelType == ModelType::Critter || _modelType == ModelType::UnarmedCritter) {
            animName.insert(0, "c");
        }
        auto &modelSceneNode = static_cast<ModelSceneNode &>(*_sceneNode);
        if (modelSceneNode.activeAnimationName() != animName) {
            auto animProps = AnimationProperties::fromFlags(AnimationFlags::loop | AnimationFlags::propagate);
            modelSceneNode.playAnimation(animName, move(animProps));
        }
    }
}

void Creature::handleClick(Object &clicker) {
    if (!_conversation.empty()) {
        _game.startConversation(_conversation);
    }
}

glm::vec3 Creature::targetWorldCoords() const {
    if (!_sceneNode) {
        return _position;
    }
    auto model = static_cast<ModelSceneNode *>(_sceneNode);
    auto headHook = model->getNodeByName("headhook");
    if (headHook) {
        return headHook->getOrigin();
    } else {
        return model->getWorldCenterOfAABB();
    }
}

void Creature::equip(int slot, IItem &item) {
    auto oldItem = _equipment.find(slot);
    if (oldItem != _equipment.end()) {
        _items.push_back(oldItem->second);
    }
    _equipment[slot] = &item;
}

void Creature::unequip(int slot) {
    auto item = _equipment.find(slot);
    if (item != _equipment.end()) {
        _items.push_back(item->second);
    }
    _equipment.erase(slot);
}

} // namespace game

} // namespace reone