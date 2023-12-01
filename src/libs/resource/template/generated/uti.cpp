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

#include "reone/resource/template/generated/uti.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

namespace generated {

static UTI_PropertiesList parseUTI_PropertiesList(const Gff &gff) {
    UTI_PropertiesList strct;
    strct.ChanceAppear = gff.getUint("ChanceAppear");
    strct.CostTable = gff.getUint("CostTable");
    strct.CostValue = gff.getUint("CostValue");
    strct.Param1 = gff.getUint("Param1");
    strct.Param1Value = gff.getUint("Param1Value");
    strct.PropertyName = gff.getUint("PropertyName");
    strct.Subtype = gff.getUint("Subtype");
    strct.UpgradeType = gff.getUint("UpgradeType");
    return strct;
}

UTI parseUTI(const Gff &gff) {
    UTI strct;
    strct.AddCost = gff.getUint("AddCost");
    strct.BaseItem = gff.getInt("BaseItem");
    strct.BodyVariation = gff.getUint("BodyVariation");
    strct.Charges = gff.getUint("Charges");
    strct.Comment = gff.getString("Comment");
    strct.Cost = gff.getUint("Cost");
    strct.DescIdentified = std::make_pair(gff.getInt("DescIdentified"), gff.getString("DescIdentified"));
    strct.Description = std::make_pair(gff.getInt("Description"), gff.getString("Description"));
    strct.Identified = gff.getUint("Identified");
    strct.LocalizedName = std::make_pair(gff.getInt("LocalizedName"), gff.getString("LocalizedName"));
    strct.ModelVariation = gff.getUint("ModelVariation");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.Plot = gff.getUint("Plot");
    for (auto &item : gff.getList("PropertiesList")) {
        strct.PropertiesList.push_back(parseUTI_PropertiesList(*item));
    }
    strct.StackSize = gff.getUint("StackSize");
    strct.Stolen = gff.getUint("Stolen");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TextureVar = gff.getUint("TextureVar");
    strct.UpgradeLevel = gff.getUint("UpgradeLevel");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
