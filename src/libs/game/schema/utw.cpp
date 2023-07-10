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

#include "reone/game/schema/utw.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace schema {

UTW parseUTW(const Gff &gff) {
    UTW strct;
    strct.Appearance = gff.getUint("Appearance");
    strct.Comment = gff.getString("Comment");
    strct.Description = std::make_pair(gff.getInt("Description"), gff.getString("Description"));
    strct.HasMapNote = gff.getUint("HasMapNote");
    strct.LinkedTo = gff.getString("LinkedTo");
    strct.LocalizedName = std::make_pair(gff.getInt("LocalizedName"), gff.getString("LocalizedName"));
    strct.MapNote = std::make_pair(gff.getInt("MapNote"), gff.getString("MapNote"));
    strct.MapNoteEnabled = gff.getUint("MapNoteEnabled");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    return strct;
}

} // namespace schema

} // namespace game

} // namespace reone
