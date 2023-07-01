# Copyright (c) 2020-2023 The reone project contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import re

from io import StringIO
from pathlib import Path
from re import Match

K1_NWSCRIPT_NSS_PATH = Path("E:/OpenKotOR/Extract/KotOR/nwscript.nss")
K2_NWSCRIPT_NSS_PATH = Path("E:/OpenKotOR/Extract/TSL/nwscript.nss")

CONST_REG_EXP = re.compile(r"^(\w+)\s+(\w+)\s*=(.*);.*")
CONST_VEC_REG_EXP = re.compile(r"^\[\s*([\.\d]+f?),\s*([\.\d]+f?),\s*([\.\d]+f?)\s*\]$")

FUNC_DECL_REG_EXP = re.compile(r"^(\w+)\s+(\w+)\s*\((.*)\);$")
FUNC_ARG_REG_EXP = re.compile(r"^(\w+)\s+(\w+)(.*)$")
FUNC_ARG_DEF_VAL_REG_EXP = re.compile(r"^(\w+)\s+(\w+)\s*=(.*)$")

REONE_COPYRIGHT_NOTICE = """/*
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
 */"""

REONE_INDENT = "    "

DEST_PATH = Path("generated")


class Constant:
    def __init__(self, type: str, name: str, value):
        self.type = type
        self.name = name
        self.value = value


class FunctionArgument:
    def __init__(self, type: str, name: str, def_val=None):
        self.type = type
        self.name = name
        self.def_val = def_val


class Function:
    def __init__(self, ret_type: str, name: str, args: list[FunctionArgument]):
        self.ret_type = ret_type
        self.name = name
        self.args = args


def parse_constant(match: Match) -> Constant:
    type = match.group(1).lower()
    name = match.group(2)
    value = match.group(3).strip()
    return Constant(type, name, value)


def parse_function_argument(str: str) -> FunctionArgument:
    match = FUNC_ARG_REG_EXP.match(str)
    if not match:
        raise RuntimeError("Invalid function argument format")
    type = match.group(1).lower()
    name = match.group(2)
    def_val_match = FUNC_ARG_DEF_VAL_REG_EXP.match(str)
    def_val = def_val_match.group(3).strip() if def_val_match else None
    return FunctionArgument(type, name, def_val)


def split_function_arguments(str: str) -> list[str]:
    args = []
    arg_chars = []
    in_str = False
    in_vec = False
    for ch in str:
        if ch == '"':
            in_str = not in_str
        if ch == "[" and not in_str:
            in_vec = True
        elif ch == "]" and not in_str:
            in_vec = False
        elif ch == "," and not in_str and not in_vec:
            arg = "".join(arg_chars).strip()
            if arg:
                args.append(arg)
            arg_chars.clear()
            continue
        arg_chars.append(ch)
    arg = "".join(arg_chars).strip()
    if arg:
        args.append(arg)
    return args


def parse_function_arguments(str: str) -> list[FunctionArgument]:
    args = []
    splitted = split_function_arguments(str)
    for arg in splitted:
        args.append(parse_function_argument(arg))
    return args


def parse_function(match: Match) -> Function:
    ret_type = match.group(1).lower()
    name = match.group(2)
    args_str = match.group(3)
    args = parse_function_arguments(args_str) if args_str else []
    return Function(ret_type, name, args)


def parse_nwscript_nss(path: Path) -> tuple[list[Constant], list[Function]]:
    constants = dict()
    functions = []
    with open(path) as f:
        for line in f:
            const_match = CONST_REG_EXP.match(line)
            if const_match:
                const = parse_constant(const_match)
                constants[const.name] = const
            func_match = FUNC_DECL_REG_EXP.match(line)
            if func_match:
                functions.append(parse_function(func_match))
    return (constants, functions)


def evaluate_constant(str: str, constants: dict):
    if str in constants:
        return constants[str].value
    else:
        vec_match = CONST_VEC_REG_EXP.match(str)
        if vec_match:
            x = vec_match.group(1)
            y = vec_match.group(2)
            z = vec_match.group(3)
            return "glm::vec3({}f, {}f, {}f)".format(x, y, z)
        else:
            return str


def write_reone_routine_get_arg_expr(
    idx: int, arg: FunctionArgument, constants: dict, code: StringIO
):
    if arg.def_val:
        evaluated = evaluate_constant(arg.def_val, constants)
        match arg.type:
            case "int":
                code.write("getIntOrElse(args, {}, {})".format(idx, evaluated))
            case "float":
                code.write("getFloatOrElse(args, {}, {})".format(idx, evaluated))
            case "string":
                code.write("getStringOrElse(args, {}, {})".format(idx, evaluated))
            case "vector":
                code.write("getVectorOrElse(args, {}, {})".format(idx, evaluated))
            case "object":
                if arg.def_val in ["0", "OBJECT_SELF"]:
                    code.write("getObjectOrCaller(args, {}, ctx)".format(idx))
                elif arg.def_val in ["1", "OBJECT_INVALID", "OBJECT_TYPE_INVALID"]:
                    code.write("getObjectOrNull(args, {}, ctx)".format(idx))
                else:
                    raise NotImplementedError(arg.def_val)
            case _:
                raise NotImplementedError(arg.type)
    else:
        match arg.type:
            case "int":
                code.write("getInt(args, {})".format(idx))
            case "float":
                code.write("getFloat(args, {})".format(idx))
            case "string":
                code.write("getString(args, {})".format(idx))
            case "vector":
                code.write("getVector(args, {})".format(idx))
            case "object":
                code.write("getObject(args, {}, ctx)".format(idx))
            case "object_id":
                code.write("getObject(args, {}, ctx)".format(idx))
            case "effect":
                code.write("getEffect(args, {})".format(idx))
            case "event":
                code.write("getEvent(args, {})".format(idx))
            case "location":
                code.write("getLocationArgument(args, {})".format(idx))
            case "talent":
                code.write("getTalent(args, {})".format(idx))
            case "action":
                code.write("getAction(args, {})".format(idx))
            case _:
                raise NotImplementedError(arg.type)


def write_reone_routine_init_arg_expr(
    idx: int, arg: FunctionArgument, constants: dict, code: StringIO
):
    code.write(REONE_INDENT + "auto " + arg.name + " = ")
    write_reone_routine_get_arg_expr(idx, arg, constants, code)
    code.write(";\n")


def write_reone_routine_impl(func: Function, constants: dict, code: StringIO):
    code.write(
        "static Variable {}(const std::vector<Variable> &args, const RoutineContext &ctx) {{\n".format(
            func.name
        )
    )
    for idx, arg in enumerate(func.args):
        write_reone_routine_init_arg_expr(idx, arg, constants, code)
    if func.args:
        code.write("\n")
    code.write(
        '{}throw RoutineNotImplementedException("{}");\n'.format(
            REONE_INDENT, func.name
        )
    )
    code.write("}\n\n")


def nss_type_to_macro(type: str) -> str:
    match type:
        case "void":
            return "R_VOID"
        case "int":
            return "R_INT"
        case "float":
            return "R_FLOAT"
        case "string":
            return "R_STRING"
        case "vector":
            return "R_VECTOR"
        case "object":
            return "R_OBJECT"
        case "object_id":
            return "R_OBJECT"
        case "effect":
            return "R_EFFECT"
        case "event":
            return "R_EVENT"
        case "location":
            return "R_LOCATION"
        case "talent":
            return "R_TALENT"
        case "action":
            return "R_ACTION"
        case _:
            raise NotImplementedError(type)


def write_reone_register_routines_func(
    category: str, game: str, functions: list[tuple[int, Function]], code: StringIO
):
    code.write(
        "void register{}{}Routines(Routines &routines) {{\n".format(category, game)
    )
    for idx, func in functions:
        code.write(
            '{}routines.insert({}, "{}", {}, {{{}}}, &routine::{});\n'.format(
                REONE_INDENT,
                idx,
                func.name,
                nss_type_to_macro(func.ret_type),
                ", ".join(map(lambda arg: nss_type_to_macro(arg.type), func.args)),
                func.name,
            )
        )
    code.write("}\n\n")


def write_reone_routine_impl_file(
    category: str,
    constants: dict,
    k1_functions: list[tuple[int, Function]],
    k2_functions: list[tuple[int, Function]],
    path: Path,
):
    code = StringIO()
    code.write(REONE_COPYRIGHT_NOTICE + "\n\n")
    code.write('#include "reone/game/script/routine/argutil.h"\n')
    code.write('#include "reone/game/script/routine/context.h"\n')
    code.write('#include "reone/game/script/routines.h"\n')
    code.write('#include "reone/script/routine/exception/notimplemented.h"\n')
    code.write('#include "reone/script/variable.h"\n')
    code.write("\n")
    code.write("#define R_VOID script::VariableType::Void\n")
    code.write("#define R_INT script::VariableType::Int\n")
    code.write("#define R_FLOAT script::VariableType::Float\n")
    code.write("#define R_OBJECT script::VariableType::Object\n")
    code.write("#define R_STRING script::VariableType::String\n")
    code.write("#define R_EFFECT script::VariableType::Effect\n")
    code.write("#define R_EVENT script::VariableType::Event\n")
    code.write("#define R_LOCATION script::VariableType::Location\n")
    code.write("#define R_TALENT script::VariableType::Talent\n")
    code.write("#define R_VECTOR script::VariableType::Vector\n")
    code.write("#define R_ACTION script::VariableType::Action\n")
    code.write("\n")
    code.write("using namespace reone::script;\n")
    code.write("\n")
    code.write("namespace reone {\n\n")
    code.write("namespace game {\n\n")
    code.write("namespace routine {\n\n")
    for _, func in k2_functions:
        write_reone_routine_impl(func, constants, code)
    code.write("} // namespace routine\n\n")
    write_reone_register_routines_func(category, "Kotor", k1_functions, code)
    write_reone_register_routines_func(category, "Tsl", k2_functions, code)
    code.write("} // namespace game\n\n")
    code.write("} // namespace reone\n\n")

    with open(path, "w") as f:
        f.write(code.getvalue())


_, k1_functions = parse_nwscript_nss(K1_NWSCRIPT_NSS_PATH)
k1_main_functions = []
k1_action_functions = []
k1_effect_functions = []
k1_minigame_functions = []
for idx, func in enumerate(k1_functions):
    if func.name.startswith("Action"):
        k1_action_functions.append((idx, func))
    elif func.name.startswith("Effect"):
        k1_effect_functions.append((idx, func))
    elif func.name.startswith("SWMG_"):
        k1_minigame_functions.append((idx, func))
    else:
        k1_main_functions.append((idx, func))

k2_constants, k2_functions = parse_nwscript_nss(K2_NWSCRIPT_NSS_PATH)
k2_main_functions = []
k2_action_functions = []
k2_effect_functions = []
k2_minigame_functions = []
for idx, func in enumerate(k2_functions):
    if func.name.startswith("Action"):
        k2_action_functions.append((idx, func))
    elif func.name.startswith("Effect"):
        k2_effect_functions.append((idx, func))
    elif func.name.startswith("SWMG_"):
        k2_minigame_functions.append((idx, func))
    else:
        k2_main_functions.append((idx, func))

DEST_PATH.mkdir(exist_ok=True)

write_reone_routine_impl_file(
    "Main", k2_constants, k1_main_functions, k2_main_functions, DEST_PATH / "main.cpp"
)
write_reone_routine_impl_file(
    "Action",
    k2_constants,
    k1_action_functions,
    k2_action_functions,
    DEST_PATH / "action.cpp",
)
write_reone_routine_impl_file(
    "Effect",
    k2_constants,
    k1_effect_functions,
    k2_effect_functions,
    DEST_PATH / "effect.cpp",
)
write_reone_routine_impl_file(
    "Minigame",
    k2_constants,
    k1_minigame_functions,
    k2_minigame_functions,
    DEST_PATH / "minigame.cpp",
)
