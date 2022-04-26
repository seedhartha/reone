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

#include "../src/script/format/ncsreader.h"
#include "../src/script/program.h"

using namespace std;

using namespace reone;
using namespace reone::script;

BOOST_AUTO_TEST_SUITE(ncs_reader)

BOOST_AUTO_TEST_CASE(should_read_ncs) {
    // given

    auto ss = ostringstream();
    ss << "NCS V1.0";
    // T
    ss << string("\x42", 1); 
    ss << string("\x00\x00\x00\x0d", 4);

    auto reader = NcsReader("");
    auto stream = make_shared<istringstream>(ss.str());

    // when

    reader.load(stream);

    // then

    auto program = reader.program();
    BOOST_CHECK_EQUAL(13, program->length());
    BOOST_CHECK_EQUAL(0ll, program->instructions().size());
}

BOOST_AUTO_TEST_SUITE_END()
