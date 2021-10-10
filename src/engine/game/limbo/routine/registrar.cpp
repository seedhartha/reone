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

#include "registrar.h"

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

void LimboRoutineRegistrar::invoke() {
    _routines.add("fabs", VT_F, {VT_F}, &routine::fabs);
    _routines.add("cos", VT_F, {VT_F}, &routine::cos);
    _routines.add("sin", VT_F, {VT_F}, &routine::sin);
    _routines.add("tan", VT_F, {VT_F}, &routine::tan);
    _routines.add("acos", VT_F, {VT_F}, &routine::acos);
    _routines.add("asin", VT_F, {VT_F}, &routine::asin);
    _routines.add("atan", VT_F, {VT_F}, &routine::atan);
    _routines.add("log", VT_F, {VT_F}, &routine::log);
    _routines.add("pow", VT_F, {VT_F, VT_F}, &routine::pow);
    _routines.add("sqrt", VT_F, {VT_F}, &routine::sqrt);
    _routines.add("abs", VT_I, {VT_I}, &routine::abs);
}

} // namespace game

} // namespace reone
