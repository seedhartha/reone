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

#include "checkutil.h"

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
    ss << string("\x0d\x00\x00\x00", 4); // number of fields
    ss << string("\xe0\x00\x00\x00", 4); // offset to labels
    ss << string("\x0d\x00\x00\x00", 4); // number of labels
    ss << string("\xb0\x01\x00\x00", 4); // offset to field data
    ss << string("\x67\x00\x00\x00", 4); // size of field data
    ss << string("\x17\x02\x00\x00", 4); // offset to field indices
    ss << string("\x34\x00\x00\x00", 4); // size of field indices
    ss << string("\x4b\x02\x00\x00", 4); // offset to list indices
    ss << string("\x00\x00\x00\x00", 4); // size of list indices

    // structs
    ss << string("\xff\xff\xff\xff", 4); // type
    ss << string("\x00\x00\x00\x00", 4); // data offset
    ss << string("\x0d\x00\x00\x00", 4); // field count

    // fields
    ss << string("\x05\x00\x00\x00", 4); // 0: type
    ss << string("\x00\x00\x00\x00", 4); // 0: label index
    ss << string("\x01\x00\x00\x00", 4); // 0: data
    ss << string("\x04\x00\x00\x00", 4); // 1: type
    ss << string("\x01\x00\x00\x00", 4); // 1: label index
    ss << string("\x02\x00\x00\x00", 4); // 1: data
    ss << string("\x07\x00\x00\x00", 4); // 2: type
    ss << string("\x02\x00\x00\x00", 4); // 2: label index
    ss << string("\x00\x00\x00\x00", 4); // 2: data
    ss << string("\x06\x00\x00\x00", 4); // 3: type
    ss << string("\x03\x00\x00\x00", 4); // 3: label index
    ss << string("\x08\x00\x00\x00", 4); // 3: data
    ss << string("\x08\x00\x00\x00", 4); // 4: type
    ss << string("\x04\x00\x00\x00", 4); // 4: label index
    ss << string("\x00\x00\x80\x3f", 4); // 4: data
    ss << string("\x09\x00\x00\x00", 4); // 5: type
    ss << string("\x05\x00\x00\x00", 4); // 5: label index
    ss << string("\x10\x00\x00\x00", 4); // 5: data
    ss << string("\x0a\x00\x00\x00", 4); // 6: type
    ss << string("\x06\x00\x00\x00", 4); // 6: label index
    ss << string("\x18\x00\x00\x00", 4); // 6: data
    ss << string("\x0b\x00\x00\x00", 4); // 7: type
    ss << string("\x07\x00\x00\x00", 4); // 7: label index
    ss << string("\x20\x00\x00\x00", 4); // 7: data
    ss << string("\x0c\x00\x00\x00", 4); // 8: type
    ss << string("\x08\x00\x00\x00", 4); // 8: label index
    ss << string("\x25\x00\x00\x00", 4); // 8: data
    ss << string("\x0d\x00\x00\x00", 4); // 9: type
    ss << string("\x09\x00\x00\x00", 4); // 9: label index
    ss << string("\x3d\x00\x00\x00", 4); // 9: data
    ss << string("\x10\x00\x00\x00", 4); // 10: type
    ss << string("\x0a\x00\x00\x00", 4); // 10: label index
    ss << string("\x43\x00\x00\x00", 4); // 10: data
    ss << string("\x11\x00\x00\x00", 4); // 11: type
    ss << string("\x0b\x00\x00\x00", 4); // 11: label index
    ss << string("\x53\x00\x00\x00", 4); // 11: data
    ss << string("\x12\x00\x00\x00", 4); // 12: type
    ss << string("\x0c\x00\x00\x00", 4); // 12: label index
    ss << string("\x5f\x00\x00\x00", 4); // 12: data

    // labels
    ss << string("Int\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Uint\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Int64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Uint64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Float\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Double\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("CExoString\x00\x00\x00\x00\x00\x00", 16);
    ss << string("ResRef\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("CExoLocString\x00\x00\x00", 16);
    ss << string("Void\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Orientation\x00\x00\x00\x00\x00", 16);
    ss << string("Vector\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("StrRef\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

    // field data
    ss << string("\x03\x00\x00\x00\x00\x00\x00\x00", 8);
    ss << string("\x04\x00\x00\x00\x00\x00\x00\x00", 8);
    ss << string("\x00\x00\x00\x00\x00\x00\xf0\x3f", 8);
    ss << string("\x04\x00\x00\x00John", 8);
    ss << string("\x04Jane", 5);
    ss << string("\x14\x00\x00\x00\xff\xff\xff\xff\x01\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00Jill", 24);
    ss << string("\x02\x00\x00\x00\xff\xff", 6);
    ss << string("\x00\x00\x80\x3f\x00\x00\x80\x3f\x00\x00\x80\x3f\x00\x00\x80\x3f", 16);
    ss << string("\x00\x00\x80\x3f\x00\x00\x80\x3f\x00\x00\x80\x3f", 12);
    ss << string("\x04\x00\x00\x00\x01\x00\x00\x00", 8);

    // field indices
    ss << string("\x00\x00\x00\x00", 4);
    ss << string("\x01\x00\x00\x00", 4);
    ss << string("\x02\x00\x00\x00", 4);
    ss << string("\x03\x00\x00\x00", 4);
    ss << string("\x04\x00\x00\x00", 4);
    ss << string("\x05\x00\x00\x00", 4);
    ss << string("\x06\x00\x00\x00", 4);
    ss << string("\x07\x00\x00\x00", 4);
    ss << string("\x08\x00\x00\x00", 4);
    ss << string("\x09\x00\x00\x00", 4);
    ss << string("\x0a\x00\x00\x00", 4);
    ss << string("\x0b\x00\x00\x00", 4);
    ss << string("\x0c\x00\x00\x00", 4);

    auto stream = make_shared<istringstream>(ss.str());
    auto reader = GffReader();
    auto expectedData = ByteArray {static_cast<char>(0xff), static_cast<char>(0xff)};
    auto expectedOrientation = glm::quat {1.0f, 1.0f, 1.0f, 1.0f};
    auto expectedVector = glm::vec3 {1.0f, 1.0f, 1.0f};

    // when

    reader.load(stream);

    // then

    auto gff = reader.root();
    BOOST_CHECK_EQUAL(13ll, gff->fields().size());
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Int), static_cast<int>(gff->fields()[0].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Dword), static_cast<int>(gff->fields()[1].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Int64), static_cast<int>(gff->fields()[2].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Dword64), static_cast<int>(gff->fields()[3].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Float), static_cast<int>(gff->fields()[4].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Double), static_cast<int>(gff->fields()[5].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::CExoString), static_cast<int>(gff->fields()[6].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::ResRef), static_cast<int>(gff->fields()[7].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::CExoLocString), static_cast<int>(gff->fields()[8].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Void), static_cast<int>(gff->fields()[9].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Orientation), static_cast<int>(gff->fields()[10].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::Vector), static_cast<int>(gff->fields()[11].type));
    BOOST_CHECK_EQUAL(static_cast<int>(GffFieldType::StrRef), static_cast<int>(gff->fields()[12].type));
    BOOST_CHECK_EQUAL(1, gff->getInt("Int"));
    BOOST_CHECK_EQUAL(2, gff->getUint("Uint"));
    BOOST_CHECK_EQUAL(3, gff->getInt64("Int64"));
    BOOST_CHECK_EQUAL(4, gff->getUint64("Uint64"));
    BOOST_CHECK_EQUAL(1.0f, gff->getFloat("Float"));
    BOOST_CHECK_EQUAL(1.0, gff->getDouble("Double"));
    BOOST_CHECK_EQUAL(string("John"), gff->getString("CExoString"));
    BOOST_CHECK_EQUAL(string("Jane"), gff->getString("ResRef"));
    BOOST_CHECK_EQUAL(-1, gff->getInt("CExoLocString"));
    BOOST_CHECK_EQUAL(string("Jill"), gff->getString("CExoLocString"));
    auto actualData = gff->getData("Void");
    BOOST_TEST((expectedData == actualData), notEqualMessage(expectedData, actualData));
    auto actualOrientation = gff->getOrientation("Orientation");
    BOOST_TEST((expectedOrientation == actualOrientation), notEqualMessage(expectedOrientation, actualOrientation));
    auto actualVector = gff->getVector("Vector");
    BOOST_TEST((expectedVector == actualVector), notEqualMessage(expectedVector, actualVector));
    BOOST_CHECK_EQUAL(1, gff->getInt("StrRef"));
}

BOOST_AUTO_TEST_SUITE_END()
