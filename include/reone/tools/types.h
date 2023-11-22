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
    ToXML,
    To2DA,
    ToGFF,
    ToRIM,
    ToERF,
    ToMOD,
    ToTLK,
    ToLIP,
    ToSSF,
    ToTGA,
    ToPCODE,
    ToNCS,
    ToNSS
};

enum class LipShape {
    Rest = 0,
    IH = 1,
    EH_ER_IY = 2,
    AA_AH_AW_AY = 3,
    AO_OW_R = 4,
    OY_UH_UW_W = 5,
    Y = 6,
    S = 7,
    F_V = 8,
    G_HH_K = 9,
    D_DH_T_TH = 10,
    B_M_P = 11,
    N_NG = 12,
    CH_JH_SH_ZH = 13,
    AE_EY = 14,
    L_Z = 15
};

} // namespace reone
