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

/** @file
 *  Appearance-related Creature functions.
 */

#include "creature.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../common/streamutil.h"
#include "../../graphics/model/models.h"
#include "../../graphics/texture/textures.h"
#include "../../resource/resources.h"
#include "../../scene/types.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static const string g_headHookNode("headhook");
static const string g_maskHookNode("gogglehook");
static const string g_rightHandNode("rhand");
static const string g_leftHandNode("lhand");

shared_ptr<ModelSceneNode> Creature::buildModel() {
    string bodyModelName(getBodyModelName());
    if (bodyModelName.empty()) return nullptr;

    shared_ptr<Model> bodyModel(Models::instance().get(bodyModelName));
    if (!bodyModel) return nullptr;

    auto bodySceneNode = make_unique<ModelSceneNode>(bodyModel, ModelUsage::Creature, _sceneGraph, this);

    // Body texture

    string bodyTextureName(getBodyTextureName());
    if (!bodyTextureName.empty()) {
        shared_ptr<Texture> texture(Textures::instance().get(bodyTextureName, TextureUsage::Diffuse));
        if (texture) {
            bodySceneNode->setDiffuseTexture(texture);
        }
    }

    // Mask

    shared_ptr<Model> maskModel;
    string maskModelName(getMaskModelName());
    if (!maskModelName.empty()) {
        maskModel = Models::instance().get(maskModelName);
    }

    // Head

    string headModelName(getHeadModelName());
    if (!headModelName.empty()) {
        shared_ptr<Model> headModel(Models::instance().get(headModelName));
        if (headModel) {
            auto headSceneNode = make_shared<ModelSceneNode>(headModel, ModelUsage::Creature, _sceneGraph, this);
            headSceneNode->setInanimateNodes(bodyModel->getAncestorNodes(g_headHookNode));
            bodySceneNode->attach(g_headHookNode, headSceneNode);
            if (maskModel) {
                auto maskSceneNode = make_shared<ModelSceneNode>(maskModel, ModelUsage::Equipment, _sceneGraph, this);
                headSceneNode->attach(g_maskHookNode, maskSceneNode);
            }
        }
    }

    // Right weapon

    string rightWeaponModelName(getWeaponModelName(InventorySlot::rightWeapon));
    if (!rightWeaponModelName.empty()) {
        shared_ptr<Model> weaponModel(Models::instance().get(rightWeaponModelName));
        if (weaponModel) {
            auto weaponSceneNode = make_shared<ModelSceneNode>(weaponModel, ModelUsage::Equipment, _sceneGraph, this);
            bodySceneNode->attach(g_rightHandNode, move(weaponSceneNode));
        }
    }

    // Left weapon

    string leftWeaponModelName(getWeaponModelName(InventorySlot::leftWeapon));
    if (!leftWeaponModelName.empty()) {
        shared_ptr<Model> weaponModel(Models::instance().get(leftWeaponModelName));
        if (weaponModel) {
            auto weaponSceneNode = make_shared<ModelSceneNode>(weaponModel, ModelUsage::Equipment, _sceneGraph, this);
            bodySceneNode->attach(g_leftHandNode, move(weaponSceneNode));
        }
    }

    return move(bodySceneNode);
}

string Creature::getBodyModelName() const {
    string column;

    if (_modelType == Creature::ModelType::Character) {
        column = "model";

        shared_ptr<Item> bodyItem(getEquippedItem(InventorySlot::body));
        if (bodyItem) {
            string baseBodyVar(bodyItem->baseBodyVariation());
            column += baseBodyVar;
        } else {
            column += "a";
        }

    } else {
        column = "race";
    }

    shared_ptr<TwoDA> appearance(Resources::instance().get2DA("appearance"));

    string modelName(appearance->getString(_appearance, column));
    boost::to_lower(modelName);

    return move(modelName);
}

string Creature::getBodyTextureName() const {
    string column;
    shared_ptr<Item> bodyItem(getEquippedItem(InventorySlot::body));

    if (_modelType == Creature::ModelType::Character) {
        column = "tex";

        if (bodyItem) {
            string baseBodyVar(bodyItem->baseBodyVariation());
            column += baseBodyVar;
        } else {
            column += "a";
        }
    } else {
        column = "racetex";
    }

    shared_ptr<TwoDA> appearance(Resources::instance().get2DA("appearance"));

    string texName(boost::to_lower_copy(appearance->getString(_appearance, column)));
    if (texName.empty()) return "";

    if (_modelType == Creature::ModelType::Character) {
        bool texFound = false;
        if (bodyItem) {
            string tmp(str(boost::format("%s%02d") % texName % bodyItem->textureVariation()));
            shared_ptr<Texture> texture(Textures::instance().get(tmp, TextureUsage::Diffuse));
            if (texture) {
                texName = move(tmp);
                texFound = true;
            }
        }
        if (!texFound) {
            texName += "01";
        }
    }

    return move(texName);
}

string Creature::getHeadModelName() const {
    if (_modelType != Creature::ModelType::Character) return "";

    shared_ptr<TwoDA> appearance(Resources::instance().get2DA("appearance"));

    int headIdx = appearance->getInt(_appearance, "normalhead", -1);
    if (headIdx == -1) return "";

    shared_ptr<TwoDA> heads(Resources::instance().get2DA("heads"));

    string modelName(heads->getString(headIdx, "head"));
    boost::to_lower(modelName);

    return move(modelName);
}

string Creature::getMaskModelName() const {
    shared_ptr<Item> headItem(getEquippedItem(InventorySlot::head));
    if (!headItem) return "";

    string modelName(boost::to_lower_copy(headItem->itemClass()));
    modelName += str(boost::format("_%03d") % headItem->modelVariation());

    return move(modelName);

}

string Creature::getWeaponModelName(int slot) const {
    shared_ptr<Item> bodyItem(getEquippedItem(slot));
    if (!bodyItem) return "";

    string modelName(bodyItem->itemClass());
    boost::to_lower(modelName);

    modelName += str(boost::format("_%03d") % bodyItem->modelVariation());

    return move(modelName);
}

} // namespace game

} // namespace reone
