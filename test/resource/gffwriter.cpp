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

#include "../../src/common/streamwriter.h"
#include "../../src/resource/format/gffwriter.h"
#include "../../src/resource/gffstruct.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(gff_writer)

BOOST_AUTO_TEST_CASE(should_write_gff) {
    // given

    auto root = make_shared<GffStruct>(
        0xffffffff,
        vector<GffField> {
            GffField::newByte("Byte", 0),
            GffField::newInt("Int", 1),
            GffField::newDword("Uint", 2),
            GffField::newInt64("Int64", 3),
            GffField::newDword64("Uint64", 4),
            GffField::newFloat("Float", 1.0f),
            GffField::newDouble("Double", 1.0),
            GffField::newCExoString("CExoString", "John"),
            GffField::newResRef("ResRef", "Jane"),
            GffField::newCExoLocString("CExoLocString", -1, "Jill"),
            GffField::newVoid("Void", ByteArray {static_cast<char>(0xff), static_cast<char>(0xff)}),
            GffField::newOrientation("Orientation", glm::quat(1.0f, 1.0f, 1.0f, 1.0f)),
            GffField::newVector("Vector", glm::vec3(1.0f, 1.0f, 1.0f)),
            GffField::newStrRef("StrRef", 1),
            GffField::newStruct(
                "Struct",
                make_shared<GffStruct>(1, vector<GffField> {GffField::newChar("Struct1Char", 1)})),
            GffField::newList(
                "List",
                vector<shared_ptr<GffStruct>> {
                    make_shared<GffStruct>(2, vector<GffField> {GffField::newWord("Struct2Word", 2)}),
                    make_shared<GffStruct>(3, vector<GffField> {GffField::newShort("Struct3Short", 3)})})});

    auto ss = ostringstream();

    // header
    ss << "RES V3.2";
    ss << string("\x38\x00\x00\x00", 4); // offset to structs
    ss << string("\x04\x00\x00\x00", 4); // number of structs
    ss << string("\x68\x00\x00\x00", 4); // offset to fields
    ss << string("\x13\x00\x00\x00", 4); // number of fields
    ss << string("\x4c\x01\x00\x00", 4); // offset to labels
    ss << string("\x13\x00\x00\x00", 4); // number of labels
    ss << string("\x7c\x02\x00\x00", 4); // offset to field data
    ss << string("\x67\x00\x00\x00", 4); // size of field data
    ss << string("\xe3\x02\x00\x00", 4); // offset to field indices
    ss << string("\x40\x00\x00\x00", 4); // size of field indices
    ss << string("\x23\x03\x00\x00", 4); // offset to list indices
    ss << string("\x0c\x00\x00\x00", 4); // size of list indices

    // structs
    ss << string("\xff\xff\xff\xff", 4); // 0: type
    ss << string("\x00\x00\x00\x00", 4); // 0: data offset
    ss << string("\x10\x00\x00\x00", 4); // 0: field count
    ss << string("\x01\x00\x00\x00", 4); // 1: type
    ss << string("\x10\x00\x00\x00", 4); // 1: data offset
    ss << string("\x01\x00\x00\x00", 4); // 1: field count
    ss << string("\x02\x00\x00\x00", 4); // 2: type
    ss << string("\x11\x00\x00\x00", 4); // 2: data offset
    ss << string("\x01\x00\x00\x00", 4); // 2: field count
    ss << string("\x03\x00\x00\x00", 4); // 3: type
    ss << string("\x12\x00\x00\x00", 4); // 3: data offset
    ss << string("\x01\x00\x00\x00", 4); // 3: field count

    // fields
    ss << string("\x00\x00\x00\x00", 4); // 0: type
    ss << string("\x00\x00\x00\x00", 4); // 0: label index
    ss << string("\x00\x00\x00\x00", 4); // 0: data
    ss << string("\x05\x00\x00\x00", 4); // 1: type
    ss << string("\x01\x00\x00\x00", 4); // 1: label index
    ss << string("\x01\x00\x00\x00", 4); // 1: data
    ss << string("\x04\x00\x00\x00", 4); // 2: type
    ss << string("\x02\x00\x00\x00", 4); // 2: label index
    ss << string("\x02\x00\x00\x00", 4); // 2: data
    ss << string("\x07\x00\x00\x00", 4); // 3: type
    ss << string("\x03\x00\x00\x00", 4); // 3: label index
    ss << string("\x00\x00\x00\x00", 4); // 3: data
    ss << string("\x06\x00\x00\x00", 4); // 4: type
    ss << string("\x04\x00\x00\x00", 4); // 4: label index
    ss << string("\x08\x00\x00\x00", 4); // 4: data
    ss << string("\x08\x00\x00\x00", 4); // 5: type
    ss << string("\x05\x00\x00\x00", 4); // 5: label index
    ss << string("\x00\x00\x80\x3f", 4); // 5: data
    ss << string("\x09\x00\x00\x00", 4); // 6: type
    ss << string("\x06\x00\x00\x00", 4); // 6: label index
    ss << string("\x10\x00\x00\x00", 4); // 6: data
    ss << string("\x0a\x00\x00\x00", 4); // 7: type
    ss << string("\x07\x00\x00\x00", 4); // 7: label index
    ss << string("\x18\x00\x00\x00", 4); // 7: data
    ss << string("\x0b\x00\x00\x00", 4); // 8: type
    ss << string("\x08\x00\x00\x00", 4); // 8: label index
    ss << string("\x20\x00\x00\x00", 4); // 8: data
    ss << string("\x0c\x00\x00\x00", 4); // 9: type
    ss << string("\x09\x00\x00\x00", 4); // 9: label index
    ss << string("\x25\x00\x00\x00", 4); // 9: data
    ss << string("\x0d\x00\x00\x00", 4); // 10: type
    ss << string("\x0a\x00\x00\x00", 4); // 10: label index
    ss << string("\x3d\x00\x00\x00", 4); // 10: data
    ss << string("\x10\x00\x00\x00", 4); // 11: type
    ss << string("\x0b\x00\x00\x00", 4); // 11: label index
    ss << string("\x43\x00\x00\x00", 4); // 11: data
    ss << string("\x11\x00\x00\x00", 4); // 12: type
    ss << string("\x0c\x00\x00\x00", 4); // 12: label index
    ss << string("\x53\x00\x00\x00", 4); // 12: data
    ss << string("\x12\x00\x00\x00", 4); // 13: type
    ss << string("\x0d\x00\x00\x00", 4); // 13: label index
    ss << string("\x5f\x00\x00\x00", 4); // 13: data
    ss << string("\x0e\x00\x00\x00", 4); // 14: type
    ss << string("\x0e\x00\x00\x00", 4); // 14: label index
    ss << string("\x01\x00\x00\x00", 4); // 14: data
    ss << string("\x0f\x00\x00\x00", 4); // 15: type
    ss << string("\x0f\x00\x00\x00", 4); // 15: label index
    ss << string("\x00\x00\x00\x00", 4); // 15: data
    ss << string("\x01\x00\x00\x00", 4); // 16: type
    ss << string("\x10\x00\x00\x00", 4); // 16: label index
    ss << string("\x01\x00\x00\x00", 4); // 16: data
    ss << string("\x02\x00\x00\x00", 4); // 17: type
    ss << string("\x11\x00\x00\x00", 4); // 17: label index
    ss << string("\x02\x00\x00\x00", 4); // 17: data
    ss << string("\x03\x00\x00\x00", 4); // 18: type
    ss << string("\x12\x00\x00\x00", 4); // 18: label index
    ss << string("\x03\x00\x00\x00", 4); // 18: data

    // labels
    ss << string("Byte\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
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
    ss << string("Struct\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("List\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
    ss << string("Struct1Char\x00\x00\x00\x00\x00", 16);
    ss << string("Struct2Word\x00\x00\x00\x00\x00", 16);
    ss << string("Struct3Short\x00\x00\x00\x00", 16);

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
    ss << string("\x0d\x00\x00\x00", 4);
    ss << string("\x0e\x00\x00\x00", 4);
    ss << string("\x0f\x00\x00\x00", 4);

    // list indices
    ss << string("\x02\x00\x00\x00", 4);
    ss << string("\x02\x00\x00\x00", 4);
    ss << string("\x03\x00\x00\x00", 4);

    auto writer = GffWriter(ResourceType::Res, root);
    auto stream = make_shared<ostringstream>();
    auto expectedOutput = ss.str();

    // when

    writer.save(stream);

    // then

    auto actualOutput = stream->str();
    BOOST_TEST((expectedOutput == actualOutput), notEqualMessage(expectedOutput, actualOutput));
}

BOOST_AUTO_TEST_SUITE_END()
