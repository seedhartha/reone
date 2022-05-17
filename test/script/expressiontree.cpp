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

#include <boost/test/unit_test.hpp>

#include "../../src/game/script/routines.h"
#include "../../src/script/expressiontree.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(expression_tree)

BOOST_AUTO_TEST_CASE(should_decompile_program) {
    // given

    auto program = ScriptProgram("");
    program.add(Instruction(13, InstructionType::RETN));

    auto routines = Routines();
    routines.initForKotOR();

    // when

    auto tree = ExpressionTree::fromProgram(program, routines);

    // then

    auto &globals = tree.globals();
    BOOST_CHECK_EQUAL(0ll, globals.size());

    auto &functions = tree.functions();
    BOOST_CHECK_EQUAL(1ll, functions.size());
    BOOST_CHECK_EQUAL("_start", functions[0]->name);
}

BOOST_AUTO_TEST_SUITE_END()
