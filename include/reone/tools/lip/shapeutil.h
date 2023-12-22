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

#include "../types.h"

namespace reone {

std::string describeLipShape(LipShape shape) {
    switch (shape) {
    case LipShape::Neutral:
        return "Neutral";
    case LipShape::IH_IY:
        return "IH, IY";
    case LipShape::EH_ER_EY:
        return "EH, ER, EY";
    case LipShape::AA_AE_AH:
        return "AA, AE, AH";
    case LipShape::OW_OY:
        return "OW, OY";
    case LipShape::UH_UW_W:
        return "UH, UW, W";
    case LipShape::D_DH_S_Y_Z:
        return "D, DH, S, Y, Z";
    case LipShape::CH_JH_SH_ZH:
        return "CH, JH, SH, ZH";
    case LipShape::F_V:
        return "F, V";
    case LipShape::G_HH_K_NG:
        return "G, HH, K, NG";
    case LipShape::T_TH:
        return "T, TH";
    case LipShape::B_M_P:
        return "B, M, P";
    case LipShape::L_N:
        return "L, N";
    case LipShape::R:
        return "R";
    case LipShape::AW_AY:
        return "AW, AY";
    case LipShape::AO:
        return "AO";
    default:
        throw std::logic_error("Unsupported LIP shape: " + std::to_string(static_cast<int>(shape)));
    }
}

} // namespace reone
