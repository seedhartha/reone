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

#pragma once

namespace reone {

namespace resource {

class Gff;

}

namespace game {

namespace gffschema {

struct UTI_PropertiesList {
    uint8_t ChanceAppear {0};
    uint8_t CostTable {0};
    uint16_t CostValue {0};
    uint8_t Param1 {0};
    uint8_t Param1Value {0};
    uint16_t PropertyName {0};
    uint16_t Subtype {0};
    uint8_t UpgradeType {0};
};

struct UTI {
    uint32_t AddCost {0};
    int BaseItem {0};
    uint8_t BodyVariation {0};
    uint8_t Charges {0};
    std::string Comment;
    uint32_t Cost {0};
    std::pair<int, std::string> DescIdentified;
    std::pair<int, std::string> Description;
    uint8_t Identified {0};
    std::pair<int, std::string> LocalizedName;
    uint8_t ModelVariation {0};
    uint8_t PaletteID {0};
    uint8_t Plot {0};
    std::vector<UTI_PropertiesList> PropertiesList;
    uint16_t StackSize {0};
    uint8_t Stolen {0};
    std::string Tag;
    std::string TemplateResRef;
    uint8_t TextureVar {0};
    uint8_t UpgradeLevel {0};
};

UTI parseUTI(const resource::Gff &gff);

} // namespace gffschema

} // namespace game

} // namespace reone
