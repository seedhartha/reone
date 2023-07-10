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

namespace schema {

struct UTS_Sounds {
    std::string Sound;
};

struct UTS {
    uint8_t Active {0};
    std::string Comment;
    uint8_t Continuous {0};
    float Elevation {0.0f};
    uint32_t Hours {0};
    uint32_t Interval {0};
    uint32_t IntervalVrtn {0};
    std::pair<int, std::string> LocName;
    uint8_t Looping {0};
    float MaxDistance {0.0f};
    float MinDistance {0.0f};
    uint8_t PaletteID {0};
    float PitchVariation {0.0f};
    uint8_t Positional {0};
    uint8_t Priority {0};
    uint8_t Random {0};
    uint8_t RandomPosition {0};
    float RandomRangeX {0.0f};
    float RandomRangeY {0.0f};
    std::vector<UTS_Sounds> Sounds;
    std::string Tag;
    std::string TemplateResRef;
    uint8_t Times {0};
    uint8_t Volume {0};
    uint8_t VolumeVrtn {0};
};

UTS parseUTS(const resource::Gff &gff);

} // namespace schema

} // namespace game

} // namespace reone
