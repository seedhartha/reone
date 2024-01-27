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

#include "reone/game/neo/eventhandler.h"

#include "reone/game/neo/event.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/graphics/options.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/provider/walkmeshes.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

void EventHandler::handle(const Event &event) {
    if (event.type == EventType::ObjectStateChanged && event.object.state == ObjectState::Loaded) {
        auto &object = _module->get().objectById(event.object.objectId)->get();
        if (object.type() == ObjectType::Area) {
            auto &area = static_cast<Area &>(object);
            onAreaLoaded(area);
        } else if (object.type() == ObjectType::Creature) {
            auto &creature = static_cast<Creature &>(object);
            onCreatureLoaded(creature);
        } else if (object.type() == ObjectType::Door) {
            auto &door = static_cast<Door &>(object);
            onDoorLoaded(door);
        } else if (object.type() == ObjectType::Placeable) {
            auto &placeable = static_cast<Placeable &>(object);
            onPlaceableLoaded(placeable);
        }
    } else if (event.type == EventType::ObjectLocationChanged) {
        auto &object = _module->get().objectById(event.object.objectId)->get();
        auto &spatial = static_cast<SpatialObject &>(object);
        onObjectLocationChanged(spatial);
    } else if (event.type == EventType::ObjectAnimationReset) {
        auto &object = _module->get().objectById(event.object.objectId)->get();
        onObjectAnimationReset(object, event.animation.name);
    } else if (event.type == EventType::ObjectFireForgetAnimationFired) {
        auto &object = _module->get().objectById(event.object.objectId)->get();
        onObjectFireForgetAnimationFired(object, event.animation.name);
    } else if (event.type == EventType::DoorStateChanged) {
        auto &object = _module->get().objectById(event.object.objectId)->get();
        auto &door = static_cast<Door &>(object);
        onDoorStateChanged(door, event.door.state);
    }
}

void EventHandler::onAreaLoaded(Area &area) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    for (auto &room : area.rooms()) {
        auto model = _resourceSvc.models.get(room.model);
        if (!model) {
            throw ResourceNotFoundException("Room model not found: " + room.model);
        }
        auto sceneNode = scene.newModel(*model, ModelUsage::Room);
        sceneNode->setLocalTransform(glm::translate(room.position));
        scene.addRoot(std::move(sceneNode));
        auto walkmesh = _resourceSvc.walkmeshes.get(room.model, ResType::Wok);
        if (walkmesh) {
            auto walkmeshSceneNode = scene.newWalkmesh(*walkmesh);
            scene.addRoot(std::move(walkmeshSceneNode));
        } else {
            warn("Room walkmesh not found: " + room.model);
        }
    }
}

void EventHandler::onCreatureLoaded(Creature &creature) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    const auto &appearance = creature.appearance();
    auto modelName = appearance.model.value();
    auto model = _resourceSvc.models.get(modelName);
    if (!model) {
        throw ResourceNotFoundException("Creature model not found: " + modelName);
    }
    auto transform = glm::translate(creature.position());
    transform *= glm::eulerAngleZ(creature.facing());
    auto sceneNode = scene.newModel(*model, ModelUsage::Creature);
    if (appearance.texture) {
        auto &texName = appearance.texture->value();
        auto texture = _resourceSvc.textures.get(texName, TextureUsage::MainTex);
        if (!texture) {
            throw ResourceNotFoundException("Creature texture not found: " + texName);
        }
        sceneNode->setMainTexture(texture.get());
    }
    if (appearance.normalHeadModel) {
        auto headModel = _resourceSvc.models.get(appearance.normalHeadModel->value());
        if (!headModel) {
            throw ResourceNotFoundException("Creature head model not found: " + modelName);
        }
        auto headSceneNode = scene.newModel(*headModel, ModelUsage::Creature);
        sceneNode->attach("headhook", *headSceneNode);
    }
    sceneNode->setLocalTransform(std::move(transform));
    sceneNode->setDrawDistance(_graphicsOpt.drawDistance);
    sceneNode->setPickable(true);
    sceneNode->setExternalRef(&creature);
    scene.addRoot(std::move(sceneNode));
}

void EventHandler::onDoorLoaded(Door &door) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto modelName = door.modelName().value();
    auto model = _resourceSvc.models.get(modelName);
    if (!model) {
        throw ResourceNotFoundException("Door model not found: " + modelName);
    }
    auto transform = glm::translate(door.position());
    transform *= glm::eulerAngleZ(door.facing());
    auto sceneNode = scene.newModel(*model, ModelUsage::Door);
    sceneNode->setLocalTransform(transform);
    // sceneNode->setDrawDistance(_graphicsOpt.drawDistance);
    sceneNode->setPickable(true);
    sceneNode->setExternalRef(&door);
    scene.addRoot(std::move(sceneNode));
    for (int i = 0; i < 3; ++i) {
        auto walkmesh = _resourceSvc.walkmeshes.get(modelName + std::to_string(i), ResType::Dwk);
        if (walkmesh) {
            auto walkmeshSceneNode = scene.newWalkmesh(*walkmesh);
            walkmeshSceneNode->setLocalTransform(transform);
            scene.addRoot(std::move(walkmeshSceneNode));
        } else {
            warn("Door walkmesh not found: " + modelName);
        }
    }
}

void EventHandler::onPlaceableLoaded(Placeable &placeable) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto modelName = placeable.modelName().value();
    auto model = _resourceSvc.models.get(modelName);
    if (!model) {
        throw ResourceNotFoundException("Placeable model not found: " + modelName);
    }
    auto transform = glm::translate(placeable.position());
    transform *= glm::eulerAngleZ(placeable.facing());
    auto sceneNode = scene.newModel(*model, ModelUsage::Placeable);
    sceneNode->setLocalTransform(transform);
    sceneNode->setDrawDistance(_graphicsOpt.drawDistance);
    sceneNode->setPickable(true);
    sceneNode->setExternalRef(&placeable);
    scene.addRoot(std::move(sceneNode));
    auto walkmesh = _resourceSvc.walkmeshes.get(modelName, ResType::Pwk);
    if (walkmesh) {
        auto walkmeshSceneNode = scene.newWalkmesh(*walkmesh);
        walkmeshSceneNode->setLocalTransform(transform);
        scene.addRoot(std::move(walkmeshSceneNode));
    } else {
        warn("Placeable walkmesh not found: " + modelName);
    }
}

void EventHandler::onObjectLocationChanged(SpatialObject &object) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto sceneNode = scene.modelByExternalRef(&object);
    if (!sceneNode) {
        return;
    }
    auto transform = glm::rotate(
        glm::translate(object.position()),
        object.facing(),
        glm::vec3 {0.0f, 0.0f, 1.0f});
    sceneNode->get().setLocalTransform(std::move(transform));
}

void EventHandler::onObjectAnimationReset(Object &object, const std::string &animName) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto sceneNode = scene.modelByExternalRef(&object);
    if (!sceneNode) {
        return;
    }
    int flags = AnimationFlags::loop |
                AnimationFlags::propagate;
    auto props = AnimationProperties::fromFlags(flags);
    sceneNode->get().playAnimation(animName, nullptr, std::move(props));
}

void EventHandler::onObjectFireForgetAnimationFired(Object &object, const std::string &animName) {
    auto &scene = _sceneSvc.graphs.get(kSceneMain);
    auto sceneNode = scene.modelByExternalRef(&object);
    if (!sceneNode) {
        return;
    }
    int flags = AnimationFlags::fireForget |
                AnimationFlags::propagate;
    auto props = AnimationProperties::fromFlags(flags);
    sceneNode->get().playAnimation(animName, nullptr, std::move(props));
}

void EventHandler::onDoorStateChanged(Door &door, DoorState state) {
    // auto &walkmeshes = _doorIdToWalkmesh.at(door.id());
    // for (size_t i = 0; i < walkmeshes.size(); ++i) {
    //     if (state == DoorState::Closed) {
    //         walkmeshes[i].get().setEnabled(i == 0);
    //     } else {
    //         walkmeshes[i].get().setEnabled(i == 1);
    //     }
    // }
}

} // namespace neo

} // namespace game

} // namespace reone
