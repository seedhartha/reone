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

#include "../../src/resource/format/erfwriter.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(erf_writer)

BOOST_AUTO_TEST_CASE(should_write_erf) {
    // given

    auto ss = ostringstream();
    // header
    ss << "ERF V1.0";
    ss << string("\x00\x00\x00\x00", 4); // number of languages
    ss << string("\x00\x00\x00\x00", 4); // size of localized strings
    ss << string("\x01\x00\x00\x00", 4); // number of entries
    ss << string("\xa0\x00\x00\x00", 4); // offset to localized strings
    ss << string("\xa0\x00\x00\x00", 4); // offset to key list
    ss << string("\xb8\x00\x00\x00", 4); // offset to resource list
    ss << string("\x00\x00\x00\x00", 4); // build year
    ss << string("\x00\x00\x00\x00", 4); // build day
    ss << string("\xff\xff\xff\xff", 4); // description strref
    ss << string(116, '\x00');           // reserved
    // key list
    ss << string("Aa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16); // resref
    ss << string("\x00\x00\x00\x00", 4);                                            // resid
    ss << string("\xe6\x07", 2);                                                    // restype
    ss << string("\x00\x00", 2);                                                    // unused
    // resource list
    ss << string("\xc0\x00\x00\x00", 4); // offset to resource
    ss << string("\x02\x00\x00\x00", 4); // resource size
    // resource data
    ss << "Bb";

    auto writer = ErfWriter();
    writer.add(ErfWriter::Resource {"Aa", ResourceType::Txi, ByteArray {'B', 'b'}});

    auto erf = make_shared<ostringstream>();
    auto expectedOutput = ss.str();

    // when

    writer.save(ErfWriter::FileType::ERF, erf);

    // then

    auto actualOutput = erf->str();
    BOOST_TEST((expectedOutput == actualOutput), notEqualMessage(expectedOutput, actualOutput));
}

BOOST_AUTO_TEST_SUITE_END()
