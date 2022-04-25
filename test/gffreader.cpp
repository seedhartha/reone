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
#include "../src/resource/format/gffreader.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(gff_reader)

BOOST_AUTO_TEST_CASE(should_read_gff) {
    // given

    auto ss = ostringstream();

    // header
    ss << "RES V3.2";
    ss << string("\x38\x00\x00\x00", 4); // offset to structs
    ss << string("\x01\x00\x00\x00", 4); // number of structs
    ss << string("\x44\x00\x00\x00", 4); // offset to fields
    ss << string("\x06\x00\x00\x00", 4); // number of fields
    ss << string("\x8c\x00\x00\x00", 4); // offset to labels
    ss << string("\x06\x00\x00\x00", 4); // number of labels
    ss << string("\xec\x00\x00\x00", 4); // offset to field data
    ss << string("\x18\x00\x00\x00", 4); // size of field data
    ss << string("\x04\x01\x00\x00", 4); // offset to field indices
    ss << string("\x18\x00\x00\x00", 4); // size of field indices
    ss << string("\x1c\x01\x00\x00", 4); // offset to list indices
    ss << string("\x00\x00\x00\x00", 4); // size of list indices

    // structs
    ss << string("\xff\xff\xff\xff", 4); // type
    ss << string("\x00\x00\x00\x00", 4); // data offset
    ss << string("\x06\x00\x00\x00", 4); // field count

    // fields
    ss << string("\x05\x00\x00\x00", 4); // type
    ss << string("\x00\x00\x00\x00", 4); // label index
    ss << string("\x01\x00\x00\x00", 4); // data
    ss << string("\x04\x00\x00\x00", 4); // type
    ss << string("\x01\x00\x00\x00", 4); // label index
    ss << string("\x02\x00\x00\x00", 4); // data
    ss << string("\x07\x00\x00\x00", 4); // type
    ss << string("\x02\x00\x00\x00", 4); // label index
    ss << string("\x00\x00\x00\x00", 4); // data
    ss << string("\x06\x00\x00\x00", 4); // type
    ss << string("\x03\x00\x00\x00", 4); // label index
    ss << string("\x08\x00\x00\x00", 4); // data
    ss << string("\x08\x00\x00\x00", 4); // type
    ss << string("\x04\x00\x00\x00", 4); // label index
    ss << string("\x00\x00\x80\x3f", 4); // data
    ss << string("\x09\x00\x00\x00", 4); // type
    ss << string("\x05\x00\x00\x00", 4); // label index
    ss << string("\x10\x00\x00\x00", 4); // data

    // labels
    ss << string("Int\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Uint\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Int64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Uint64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Float\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Double\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

    // field data
    ss << string("\x03\x00\x00\x00\x00\x00\x00\x00", 8);
    ss << string("\x04\x00\x00\x00\x00\x00\x00\x00", 8);
    ss << string("\x00\x00\x00\x00\x00\x00\xf0\x3f", 8);

    // field indices
    ss << string("\x00\x00\x00\x00", 4);
    ss << string("\x01\x00\x00\x00", 4);
    ss << string("\x02\x00\x00\x00", 4);
    ss << string("\x03\x00\x00\x00", 4);
    ss << string("\x04\x00\x00\x00", 4);
    ss << string("\x05\x00\x00\x00", 4);

    auto stream = make_shared<istringstream>(ss.str());
    auto reader = GffReader();

    // when

    reader.load(stream);

    // then

    auto gff = reader.root();
    BOOST_CHECK_EQUAL(6ll, gff->fields().size());
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Int), static_cast<int>(gff->fields()[0].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Dword), static_cast<int>(gff->fields()[1].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Int64), static_cast<int>(gff->fields()[2].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Dword64), static_cast<int>(gff->fields()[3].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Float), static_cast<int>(gff->fields()[4].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Double), static_cast<int>(gff->fields()[5].type));
    BOOST_CHECK_EQUAL(1, gff->getInt("Int"));
    BOOST_CHECK_EQUAL(2, gff->getUint("Uint"));
    BOOST_CHECK_EQUAL(3, gff->getInt64("Int64"));
    BOOST_CHECK_EQUAL(4, gff->getUint64("Uint64"));
    BOOST_CHECK_EQUAL(1.0f, gff->getFloat("Float"));
    BOOST_CHECK_EQUAL(1.0, gff->getDouble("Double"));
}

BOOST_AUTO_TEST_SUITE_END()
