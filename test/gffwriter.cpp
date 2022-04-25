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

#include "../src/common/streamwriter.h"
#include "../src/resource/format/gffwriter.h"
#include "../src/resource/gffstruct.h"

#include "checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(gff_writer)

BOOST_AUTO_TEST_CASE(should_write_gff) {
    // given

    auto root = make_shared<GffStruct>(0xffffffff, vector<GffField> { GffField::newByte("Name", 1) });
    auto writer = GffWriter(ResourceType::Res, root);

    auto ss = ostringstream();

    // header
    ss << "RES V3.2";
    ss << string("\x38\x00\x00\x00", 4); // offset to structs
    ss << string("\x01\x00\x00\x00", 4); // number of structs
    ss << string("\x44\x00\x00\x00", 4); // offset to fields
    ss << string("\x01\x00\x00\x00", 4); // number of fields
    ss << string("\x50\x00\x00\x00", 4); // offset to labels
    ss << string("\x01\x00\x00\x00", 4); // number of labels
    ss << string("\x60\x00\x00\x00", 4); // offset to field data
    ss << string("\x00\x00\x00\x00", 4); // size of field data
    ss << string("\x60\x00\x00\x00", 4); // offset to field indices
    ss << string("\x00\x00\x00\x00", 4); // size of field indices
    ss << string("\x60\x00\x00\x00", 4); // offset to list indices
    ss << string("\x00\x00\x00\x00", 4); // size of list indices

    // structs
    ss << string("\xff\xff\xff\xff", 4); // type
    ss << string("\x00\x00\x00\x00", 4); // data offset
    ss << string("\x01\x00\x00\x00", 4); // field count

    // fields
    ss << string("\x00\x00\x00\x00", 4); // type
    ss << string("\x00\x00\x00\x00", 4); // label index
    ss << string("\x01\x00\x00\x00", 4); // data

    // labels
    ss << string("Name\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

    auto stream = make_shared<ostringstream>();
    auto expectedOutput = ss.str();

    // when

    writer.save(stream);

    // then

    auto actualOutput = stream->str();
    BOOST_TEST(expectedOutput == actualOutput, notEqualMessage(expectedOutput, actualOutput));
}

BOOST_AUTO_TEST_SUITE_END()
