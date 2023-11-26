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

namespace generated {

struct UTW {
    uint8_t Appearance {0};
    std::string Comment;
    std::pair<int, std::string> Description;
    uint8_t HasMapNote {0};
    std::string LinkedTo;
    std::pair<int, std::string> LocalizedName;
    std::pair<int, std::string> MapNote;
    uint8_t MapNoteEnabled {0};
    uint8_t PaletteID {0};
    std::string Tag;
    std::string TemplateResRef;
};

UTW parseUTW(const resource::Gff &gff);

} // namespace generated

} // namespace game

} // namespace reone
