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

#include "reone/game/neo/object/creature.h"

#include "reone/game/neo/object/item.h"
#include "reone/resource/2da.h"
#include "reone/resource/template/generated/utc.h"
#include "reone/system/exception/validation.h"

using namespace reone::resource;
using namespace reone::resource::generated;

namespace reone {

namespace game {

namespace neo {

enum class ModelType {
    FullBody,
    BodyAndHead,
    S,
    L
};

void Creature::load(const UTC &utc,
                    const TwoDA &appearance2da,
                    const TwoDA &heads2da) {
    ModelType modelType;
    auto modelTypeStr = appearance2da.getString(utc.Appearance_Type, "modeltype");
    if (modelTypeStr == "F") {
        modelType = ModelType::FullBody;
    } else if (modelTypeStr == "B") {
        modelType = ModelType::BodyAndHead;
    } else if (modelTypeStr == "S") {
        modelType = ModelType::S;
    } else if (modelTypeStr == "L") {
        modelType = ModelType::L;
    } else {
        throw ValidationException("Unexpected model type: " + modelTypeStr);
    }

    std::string modelColumn;
    std::string texture;
    if (modelType == ModelType::BodyAndHead) {
        char bodyVarChar = 'a' + std::max(0, (utc.BodyVariation - 1));
        modelColumn = "model";
        modelColumn.push_back(bodyVarChar);

        std::string texColumn {"tex"};
        texColumn.push_back(bodyVarChar);
        texture = appearance2da.getString(utc.Appearance_Type, texColumn);
        texture.append(str(boost::format("%02d") % std::max(1, static_cast<int>(utc.TextureVar))));

        uint32_t normalhead = appearance2da.getUint(utc.Appearance_Type, "normalhead");
        auto normalHeadModel = heads2da.getString(normalhead, "head");
        if (normalHeadModel.empty()) {
            throw ValidationException("Empty normal head model name");
        }
        _appearance.normalHeadModel = std::move(normalHeadModel);

        uint32_t backuphead = appearance2da.getUint(utc.Appearance_Type, "backuphead");
        if (backuphead != 0) {
            auto backupHeadModel = heads2da.getString(backuphead, "head");
            if (backupHeadModel.empty()) {
                throw ValidationException("Empty backup head model name");
            }
            _appearance.backupHeadModel = std::move(backupHeadModel);
        }
    } else {
        modelColumn = "race";
        texture = appearance2da.getString(utc.Appearance_Type, "racetex");
    }
    auto model = appearance2da.getString(utc.Appearance_Type, modelColumn);
    if (model.empty()) {
        throw ValidationException("Empty model name");
    }
    _appearance.model = std::move(model);
    _appearance.texture = std::move(texture);

    _state = ObjectState::Loaded;
}

void Creature::equip(Item &item, InventorySlot slot) {
    auto it = _equipment.find(slot);
    if (it == _equipment.end()) {
        _equipment.insert({slot, item});
    } else {
        _inventory.add(it->second);
        _equipment.at(slot) = item;
    }
}

void Creature::unequip(const Item &item) {
    for (auto it = _equipment.begin(); it != _equipment.end(); ++it) {
        auto &equipped = it->second.get();
        if (equipped == item) {
            _inventory.add(equipped);
            _equipment.erase(it);
            break;
        }
    }
}

} // namespace neo

} // namespace game

} // namespace reone
