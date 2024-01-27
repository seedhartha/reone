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
                    const AppearanceTwoDA &appearance,
                    const HeadsTwoDA &heads) {
    load(
        utc.Appearance_Type,
        appearance,
        heads,
        utc.BodyVariation,
        utc.TextureVar);
}

static std::string modelFromAppearance(const AppearanceTwoDARow &row, const std::string &column) {
    if (column == "race") {
        return row.race.value_or("");
    }
    if (column == "modela") {
        return row.modela.value_or("");
    }
    if (column == "modelb") {
        return row.modelb.value_or("");
    }
    if (column == "modelc") {
        return row.modelc.value_or("");
    }
    if (column == "modeld") {
        return row.modeld.value_or("");
    }
    if (column == "modele") {
        return row.modele.value_or("");
    }
    if (column == "modelf") {
        return row.modelf.value_or("");
    }
    if (column == "modelg") {
        return row.modelg.value_or("");
    }
    if (column == "modelh") {
        return row.modelh.value_or("");
    }
    if (column == "modeli") {
        return row.modeli.value_or("");
    }
    if (column == "modelj") {
        return row.modelj.value_or("");
    }
    if (column == "modelk") {
        return row.modelk.value_or("");
    }
    if (column == "modell") {
        return row.modell.value_or("");
    }
    if (column == "modelm") {
        return row.modelm.value_or("");
    }
    if (column == "modeln") {
        return row.modeln.value_or("");
    }
    throw std::invalid_argument("Unsupported model column: " + column);
}

static std::string textureFromAppearance(const AppearanceTwoDARow &row, const std::string &column) {
    if (column == "racetex") {
        return row.racetex.value_or("");
    }
    if (column == "texa") {
        return row.texa.value_or("");
    }
    if (column == "texb") {
        return row.texb.value_or("");
    }
    if (column == "texc") {
        return row.texc.value_or("");
    }
    if (column == "texd") {
        return row.texd.value_or("");
    }
    if (column == "texe") {
        return row.texe.value_or("");
    }
    if (column == "texf") {
        return row.texf.value_or("");
    }
    if (column == "texg") {
        return row.texg.value_or("");
    }
    if (column == "texh") {
        return row.texh.value_or("");
    }
    if (column == "texi") {
        return row.texi.value_or("");
    }
    if (column == "texj") {
        return row.texj.value_or("");
    }
    if (column == "texk") {
        return row.texk.value_or("");
    }
    if (column == "texl") {
        return row.texl.value_or("");
    }
    if (column == "texm") {
        return row.texm.value_or("");
    }
    if (column == "texn") {
        return row.texn.value_or("");
    }
    throw std::invalid_argument("Unsupported texture column: " + column);
}

void Creature::load(AppearanceId appearanceId,
                    const AppearanceTwoDA &appearance,
                    const HeadsTwoDA &heads,
                    std::optional<int> bodyVariation,
                    std::optional<int> texVariation) {
    const auto &appearanceRow = appearance.rows[appearanceId];

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
        texture = textureFromAppearance(appearanceRow, texColumn);
        if (!texture.empty()) {
            texture.append(str(boost::format("%02d") % std::max(1, static_cast<int>(texVariation.value_or(1)))));
        }

        if (!appearanceRow.normalhead) {
            throw ValidationException("Normal head not defined");
        }
        const auto &normalHeadRow = heads.rows[*appearanceRow.normalhead];
        if (!normalHeadRow.head) {
            throw ValidationException("Empty normal head model name");
        }
        _appearance.normalHeadModel = *normalHeadRow.head;

        if (appearanceRow.backuphead) {
            const auto &backupHeadRow = heads.rows[*appearanceRow.backuphead];
            if (!backupHeadRow.head) {
                throw ValidationException("Empty backup head model name");
            }
            _appearance.backupHeadModel = *backupHeadRow.head;
        }
    } else {
        modelColumn = "race";
        texture = appearanceRow.racetex.value_or("");
    }
    auto model = modelFromAppearance(appearanceRow, modelColumn);
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

    switch (_moveType) {
    case MoveType::None:
        resetAnimation("pause1");
        break;
    case MoveType::Walk:
        resetAnimation("walk");
        break;
    case MoveType::Run:
        resetAnimation("run");
        break;
    default:
        throw std::invalid_argument("Unsupported move type: " + std::to_string(static_cast<int>(type)));
    }
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
