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

struct UTE_CreatureList {
    int Appearance {0};
    float CR {0.0f};
    int GuaranteedCount {0};
    std::string ResRef;
    uint8_t SingleSpawn {0};
};

struct UTE {
    uint8_t Active {0};
    std::string Comment;
    std::vector<UTE_CreatureList> CreatureList;
    int Difficulty {0};
    int DifficultyIndex {0};
    uint32_t Faction {0};
    std::pair<int, std::string> LocalizedName;
    int MaxCreatures {0};
    std::string OnEntered;
    std::string OnExhausted;
    std::string OnExit;
    std::string OnHeartbeat;
    std::string OnUserDefined;
    uint8_t PaletteID {0};
    uint8_t PlayerOnly {0};
    int RecCreatures {0};
    uint8_t Reset {0};
    int ResetTime {0};
    int Respawns {0};
    int SpawnOption {0};
    std::string Tag;
    std::string TemplateResRef;
};

UTE parseUTE(const resource::Gff &gff);

} // namespace generated

} // namespace game

} // namespace reone
