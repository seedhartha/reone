/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "registerutil.h"

#include "../../../script/types.h"

#include "../../core/script/routine/declarations.h"
#include "../../core/script/routine/iroutines.h"

namespace reone {

namespace game {

#define VT_V script::VariableType::Void
#define VT_I script::VariableType::Int
#define VT_F script::VariableType::Float
#define VT_O script::VariableType::Object
#define VT_S script::VariableType::String
#define VT_EFFECT script::VariableType::Effect
#define VT_EVENT script::VariableType::Event
#define VT_LOCATION script::VariableType::Location
#define VT_TALENT script::VariableType::Talent
#define VT_VECTOR script::VariableType::Vector
#define VT_ACTION script::VariableType::Action

void registerRoutinesLimbo(IRoutines &routines) {
    routines.add("fabs", VT_F, {VT_F}, &routine::fabs);
    routines.add("cos", VT_F, {VT_F}, &routine::cos);
    routines.add("sin", VT_F, {VT_F}, &routine::sin);
    routines.add("tan", VT_F, {VT_F}, &routine::tan);
    routines.add("acos", VT_F, {VT_F}, &routine::acos);
    routines.add("asin", VT_F, {VT_F}, &routine::asin);
    routines.add("atan", VT_F, {VT_F}, &routine::atan);
    routines.add("log", VT_F, {VT_F}, &routine::log);
    routines.add("pow", VT_F, {VT_F, VT_F}, &routine::pow);
    routines.add("sqrt", VT_F, {VT_F}, &routine::sqrt);
    routines.add("abs", VT_I, {VT_I}, &routine::abs);

    routines.add("StartNewModule", VT_V, {VT_S, VT_S, VT_S, VT_S, VT_S, VT_S, VT_S, VT_S}, &routine::startNewModule);
}

} // namespace game

} // namespace reone
