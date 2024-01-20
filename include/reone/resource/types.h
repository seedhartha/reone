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

namespace audio {

class AudioClip;

}

namespace resource {

enum class ResType : uint16_t {
    Res = 0,
    Bmp = 1,
    Tga = 3,
    Wav = 4,
    Plt = 6,
    Ini = 7,
    Txt = 10,
    Mdl = 2002,
    Nss = 2009,
    Ncs = 2010,
    Are = 2012,
    Set = 2013,
    Ifo = 2014,
    Bic = 2015,
    Wok = 2016,
    TwoDA = 2017,
    Tlk = 2018,
    Txi = 2022,
    Git = 2023,
    Bti = 2024,
    Uti = 2025,
    Btc = 2026,
    Utc = 2027,
    Dlg = 2029,
    Itp = 2030,
    Utt = 2032,
    Dds = 2033,
    Uts = 2035,
    Ltr = 2036,
    Gff = 2037,
    Fac = 2038,
    Ute = 2040,
    Utd = 2042,
    Utp = 2044,
    Dft = 2045,
    Gic = 2046,
    Gui = 2047,
    Utm = 2051,
    Dwk = 2052,
    Pwk = 2053,
    Jrl = 2056,
    Mod = 2057,
    Utw = 2058,
    Ssf = 2060,
    Ndb = 2064,
    Ptm = 2065,
    Ptt = 2066,
    Lyt = 3000,
    Vis = 3001,
    Pth = 3003,
    Lip = 3004,
    Tpc = 3007,
    Mdx = 3008,

    // Mods and engine-specific
    Mp3 = 0x1000,
    Glsl = 0x1001,

    // Windows executable
    Cursor = 0x2000,
    CursorGroup = 0x2001,

    Invalid = 0xffff
};

enum class GameID {
    KotOR,
    TSL
};

enum class ConversationType {
    Cinematic = 0,
    Computer = 1
};

enum class ComputerType {
    Normal = 0,
    Rakatan = 1
};

enum class SoundSetEntry {
    BattleCry1 = 0,
    BattleCry2 = 1,
    BattleCry3 = 2,
    BattleCry4 = 3,
    BattleCry5 = 4,
    BattleCry6 = 5,
    Select1 = 6,
    Select2 = 7,
    Select3 = 8,
    AttackGrunt1 = 9,
    AttackGrunt2 = 10,
    AttackGrunt3 = 11,
    PainGrunt1 = 12,
    PainGrunt2 = 13,
    LowHealth = 14,
    Dead = 15,
    CriticalHit = 16,
    TargetImmune = 17,
    LayMine = 18,
    DisarmMine = 19,
    BeginStealth = 20,
    BeginSearch = 21,
    BeginUnlock = 22,
    UnlockFailed = 23,
    UnlockSuccess = 24,
    SeparatedFromParty = 25,
    RejoinParty = 26,
    Poisoned = 27
};

enum class CursorType {
    None,
    Default,
    Attack,
    Door,
    Talk,
    Pickup,
    DisableMine,
    RecoverMine
};

using Visibility = std::multimap<std::string, std::string>;
using SoundSet = std::unordered_map<SoundSetEntry, std::shared_ptr<reone::audio::AudioClip>>;

} // namespace resource

} // namespace reone
