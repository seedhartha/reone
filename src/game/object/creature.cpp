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

#include "reone/common/exception/validation.h"
#include "reone/common/logutil.h"
#include "reone/graphics/models.h"
#include "reone/graphics/services.h"
#include "reone/graphics/textures.h"
#include "reone/resource/2das.h"
#include "reone/resource/gffs.h"
#include "reone/resource/services.h"
#include "reone/resource/strings.h"
#include "reone/scene/collision.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/model.h"

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
static const string kRightHandNodeName = "rhand";
static const string kLeftHandNodeName = "lhand";
static const string kGoggleHookNodeName = "gogglehook";

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
    auto bodyVariation = utc->getInt("BodyVariation", 1);
    auto textureVar = utc->getInt("TextureVar", 1);

    auto itemList = utc->getList("ItemList");
    for (auto &utcItem : itemList) {
        auto inventoryRes = utcItem->getString("InventoryRes");
        auto item = static_pointer_cast<Item>(_objectFactory.newItem());
        item->setSceneGraph(_sceneGraph);
        item->loadFromUti(inventoryRes);
        _items.push_back(item.get());
    }

    auto equipItemList = utc->getList("Equip_ItemList");
    for (auto &utcItem : equipItemList) {
        auto equippedRes = utcItem->getString("EquippedRes");
        auto item = static_pointer_cast<Item>(_objectFactory.newItem());
        item->setSceneGraph(_sceneGraph);
        item->loadFromUti(equippedRes);
        if (!equip(*item)) {
            _items.push_back(item.get());
        }
    }

    // From appearance 2DA

    auto appearanceTable = _resourceSvc.twoDas.get("appearance");
    if (!appearanceTable) {
        throw ValidationException("appearance 2DA not found");
    }
    auto modelType = appearanceTable->getString(appearanceType, "modeltype");
    auto race = appearanceTable->getString(appearanceType, "race");
    auto raceTex = appearanceTable->getString(appearanceType, "racetex");
    auto modelName = race;
    auto textureName = raceTex;
    if (modelType == "B") {
        string modelColumn = "model";
        string textureColumn = "tex";
        auto bodyEquipment = _equipment.find(InventorySlot::body);
        if (bodyEquipment != _equipment.end()) {
            auto itemImpl = static_cast<Item *>(bodyEquipment->second);
            modelColumn += 'a' + itemImpl->bodyVariation() - 1;
            textureColumn += 'a' + itemImpl->bodyVariation() - 1;
        } else {
            modelColumn += 'a' + bodyVariation - 1;
            textureColumn += 'a' + bodyVariation - 1;
        }
        modelName = appearanceTable->getString(appearanceType, modelColumn);
        textureName = appearanceTable->getString(appearanceType, textureColumn);
        if (bodyEquipment != _equipment.end()) {
            auto itemImpl = static_cast<Item *>(bodyEquipment->second);
            textureName += str(boost::format("%02d") % itemImpl->textureVar());
        } else {
            textureName += str(boost::format("%02d") % textureVar);
        }
    }

    // Make scene node

    shared_ptr<ModelSceneNode> sceneNode;
    auto model = _graphicsSvc.models.get(modelName);
    if (model) {
        sceneNode = _sceneGraph->newModel(*model, ModelUsage::Creature);
        sceneNode->init();

        // Head
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

                // Mask
                auto headEquipment = _equipment.find(InventorySlot::head);
                if (headEquipment != _equipment.end()) {
                    auto itemImpl = static_cast<Item *>(headEquipment->second);
                    auto maskSceneNode = itemImpl->sceneNode();
                    if (maskSceneNode) {
                        headSceneNode->attach(kGoggleHookNodeName, *maskSceneNode);
                    }
                }
            }
        }

        // Weapons
        auto rhandEquipment = _equipment.find(InventorySlot::rightWeapon);
        if (rhandEquipment != _equipment.end()) {
            auto itemImpl = static_cast<Item *>(rhandEquipment->second);
            auto rhandSceneNode = itemImpl->sceneNode();
            if (rhandSceneNode) {
                sceneNode->attach(kRightHandNodeName, *rhandSceneNode);
            }
        }
        auto lhandEquipment = _equipment.find(InventorySlot::leftWeapon);
        if (lhandEquipment != _equipment.end()) {
            auto itemImpl = static_cast<Item *>(lhandEquipment->second);
            auto lhandSceneNode = itemImpl->sceneNode();
            if (lhandSceneNode) {
                sceneNode->attach(kLeftHandNodeName, *lhandSceneNode);
            }
        }

        auto texture = _graphicsSvc.textures.get(textureName);
        if (texture) {
            sceneNode->setDiffuseMap(texture.get());
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

bool Creature::equip(IItem &item) {
    auto &itemImpl = static_cast<Item &>(item);
    auto slots = set<int> {
        InventorySlot::head,
        InventorySlot::body,
        InventorySlot::hands,
        InventorySlot::rightWeapon,
        InventorySlot::leftWeapon,
        InventorySlot::leftArm,
        InventorySlot::rightArm,
        InventorySlot::implant,
        InventorySlot::belt,
        InventorySlot::cWeaponL,
        InventorySlot::cWeaponR,
        InventorySlot::cWeaponB,
        InventorySlot::cArmour,
        InventorySlot::rightWeapon2,
        InventorySlot::leftWeapon2};
    for (auto &slot : slots) {
        if (itemImpl.isEquipable(slot) && _equipment.count(slot) == 0) {
            return equip(slot, item);
        }
    }
    return false;
}

bool Creature::equip(int slot, IItem &item) {
    auto &itemImpl = static_cast<Item &>(item);
    if (!itemImpl.isEquipable(slot)) {
        return false;
    }
    auto oldItem = _equipment.find(slot);
    if (oldItem != _equipment.end()) {
        _items.push_back(oldItem->second);
    }
    _equipment[slot] = &item;
    return true;
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