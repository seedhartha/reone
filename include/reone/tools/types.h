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

enum class Operation {
    None,
    List,
    Extract,
    Unwrap,
    ToRIM,
    ToERF,
    ToMOD,
    ToTGA,
    ToPCODE,
    ToNCS,
    ToNSS
};

enum class LipShape {
    Neutral = 0,
    IH_IY = 1,
    EH_ER_EY = 2,
    AA_AE_AH = 3,
    OW_OY = 4,
    UH_UW_W = 5,
    D_DH_S_Y_Z = 6,
    CH_JH_SH_ZH = 7,
    F_V = 8,
    G_HH_K_NG = 9,
    T_TH = 10,
    B_M_P = 11,
    L_N = 12,
    R = 13,
    AW_AY = 14,
    AO = 15
};

} // namespace reone
