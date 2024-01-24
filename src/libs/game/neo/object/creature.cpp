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
#include "reone/resource/parser/2da/appearance.h"
#include "reone/resource/parser/2da/heads.h"
#include "reone/resource/parser/gff/utc.h"
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
                    const TwoDA &appearance,
                    const TwoDA &heads) {
    load(
        utc.Appearance_Type,
        appearance,
        heads,
        utc.BodyVariation,
        utc.TextureVar);
}

void Creature::load(AppearanceId appearanceId,
                    const resource::TwoDA &appearance,
                    const resource::TwoDA &heads,
                    std::optional<int> bodyVariation,
                    std::optional<int> texVariation) {
    auto appearanceRow = parse_appearance(appearance, appearanceId);

    ModelType modelType;
    if (appearanceRow.modeltype == "F") {
        modelType = ModelType::FullBody;
    } else if (appearanceRow.modeltype == "B") {
        modelType = ModelType::BodyAndHead;
    } else if (appearanceRow.modeltype == "S") {
        modelType = ModelType::S;
    } else if (appearanceRow.modeltype == "L") {
        modelType = ModelType::L;
    } else {
        throw ValidationException("Unexpected creature model type: " + appearanceRow.modeltype);
    }

    std::string modelColumn;
    std::string texture;
    if (modelType == ModelType::BodyAndHead) {
        char bodyVarChar = 'a' + std::max(0, (bodyVariation.value_or(1) - 1));
        modelColumn = "model";
        modelColumn.push_back(bodyVarChar);

        std::string texColumn {"tex"};
        texColumn.push_back(bodyVarChar);
        texture = appearance.getString(appearanceId, texColumn);
        if (!texture.empty()) {
            texture.append(str(boost::format("%02d") % std::max(1, static_cast<int>(texVariation.value_or(1)))));
        }

        if (!appearanceRow.normalhead) {
            throw ValidationException("Normal head not defined");
        }
        auto normalHeadRow = parse_heads(heads, *appearanceRow.normalhead);
        if (!normalHeadRow.head) {
            throw ValidationException("Empty normal head model name");
        }
        _appearance.normalHeadModel = *normalHeadRow.head;

        if (appearanceRow.backuphead) {
            auto backupHeadRow = parse_heads(heads, *appearanceRow.backuphead);
            if (!backupHeadRow.head) {
                throw ValidationException("Empty backup head model name");
            }
            _appearance.backupHeadModel = *backupHeadRow.head;
        }
    } else {
        modelColumn = "race";
        texture = appearanceRow.racetex.value_or("");
    }
    auto model = appearance.getString(appearanceId, modelColumn);
    if (model.empty()) {
        throw ValidationException("Empty creature model name");
    }
    _appearance.model = std::move(model);
    if (!texture.empty()) {
        _appearance.texture = std::move(texture);
    }

    setState(ObjectState::Loaded);
    setMoveType(MoveType::None);
}

void Creature::setMoveType(MoveType type) {
    if (_moveType == type) {
        return;
    }
    _moveType = type;

    Event event;
    event.type = EventType::ObjectAnimationChanged;
    event.animation.objectId = _id;
    switch (_moveType) {
    case MoveType::None:
        event.animation.name = "pause1";
        break;
    case MoveType::Walk:
        event.animation.name = "walk";
        break;
    case MoveType::Run:
        event.animation.name = "run";
        break;
    default:
        throw std::invalid_argument("Unsupported move type: " + std::to_string(static_cast<int>(type)));
    }
    _events.push_back(std::move(event));
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
