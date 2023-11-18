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

struct UTT {
    uint8_t AutoRemoveKey {0};
    std::string Comment;
    uint8_t Cursor {0};
    uint8_t DisarmDC {0};
    uint32_t Faction {0};
    float HighlightHeight {0.0f};
    std::string KeyName;
    std::string LinkedTo;
    uint8_t LinkedToFlags {0};
    uint16_t LoadScreenID {0};
    std::pair<int, std::string> LocalizedName;
    std::string OnClick;
    std::string OnDisarm;
    std::string OnTrapTriggered;
    uint8_t PaletteID {0};
    uint8_t PartyRequired {0};
    std::string Portrait;
    uint16_t PortraitId {0};
    std::string ScriptHeartbeat;
    std::string ScriptOnEnter;
    std::string ScriptOnExit;
    std::string ScriptUserDefine;
    std::string Tag;
    std::string TemplateResRef;
    uint8_t TrapDetectDC {0};
    uint8_t TrapDetectable {0};
    uint8_t TrapDisarmable {0};
    uint8_t TrapFlag {0};
    uint8_t TrapOneShot {0};
    uint8_t TrapType {0};
    int Type {0};
};

UTT parseUTT(const resource::Gff &gff);

} // namespace gffschema

} // namespace game

} // namespace reone
