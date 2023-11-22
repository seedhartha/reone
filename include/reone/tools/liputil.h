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

#include "types.h"

namespace reone {

std::string describeLipShape(LipShape shape) {
    switch (shape) {
    case LipShape::Rest:
        return "Rest";
    case LipShape::IH:
        return "IH";
    case LipShape::EH_ER_IY:
        return "EH_ER_IY";
    case LipShape::AA_AH_AW_AY:
        return "AA_AH_AW_AY";
    case LipShape::AO_OW_R:
        return "AO_OW_R";
    case LipShape::OY_UH_UW_W:
        return "OY_UH_UW_W";
    case LipShape::Y:
        return "Y";
    case LipShape::S:
        return "S";
    case LipShape::F_V:
        return "F_V";
    case LipShape::G_HH_K:
        return "G_HH_K";
    case LipShape::D_DH_T_TH:
        return "D_DH_T_TH";
    case LipShape::B_M_P:
        return "B_M_P";
    case LipShape::N_NG:
        return "N_NG";
    case LipShape::CH_JH_SH_ZH:
        return "CH_JH_SH_ZH";
    case LipShape::AE_EY:
        return "AE_EY";
    case LipShape::L_Z:
        return "L_Z";
    default:
        throw std::logic_error("Unsupported LIP shape: " + std::to_string(static_cast<int>(shape)));
    }
}

} // namespace reone
