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

using namespace std;

using namespace reone;

BOOST_AUTO_TEST_SUITE(stream_writer)

BOOST_AUTO_TEST_CASE(should_put_numbers_to_little_endian_stream) {
    // given
    auto stream = make_shared<ostringstream>();
    auto writer = StreamWriter(stream, boost::endian::order::little);

    // when
    writer.putByte(0x40);
    writer.putChar('A');
    writer.putUint16(1);
    writer.putUint32(2);
    writer.putInt16(-3);
    writer.putInt32(-4);
    writer.putInt64(-5);
    writer.putFloat(1.0f);

    // then
    auto output = stream->str();
    BOOST_CHECK_EQUAL(string("\x40\x41\x01\x00\x02\x00\x00\x00\xfd\xff\xfc\xff\xff\xff\xfb\xff\xff\xff\xff\xff\xff\xff\x00\x00\x80\x3f", 26), output);
}

BOOST_AUTO_TEST_CASE(should_put_strings_to_little_endian_stream) {
    // given
    auto stream = make_shared<ostringstream>();
    auto writer = StreamWriter(stream, boost::endian::order::little);

    // when
    writer.putString(string("Aa"));
    writer.putStringExact(string("Bb"), 2);
    writer.putCString(string("Cc\x00"));

    // then
    auto output = stream->str();
    BOOST_CHECK_EQUAL(string("AaBbCc\x00", 7), output);
}

BOOST_AUTO_TEST_CASE(should_put_bytes_to_little_endian_stream) {
    // given
    auto stream = make_shared<ostringstream>();
    auto writer = StreamWriter(stream, boost::endian::order::little);
    auto bytes = ByteArray({ 0x01, 0x02, 0x03, 0x04 });

    // when
    writer.putBytes(bytes);

    // then
    auto output = stream->str();
    BOOST_CHECK_EQUAL(string("\x01\x02\x03\x04", 4), output);
}

BOOST_AUTO_TEST_CASE(should_put_numbers_to_big_endian_stream) {
    // given
    auto stream = make_shared<ostringstream>();
    auto writer = StreamWriter(stream, boost::endian::order::big);

    // when
    writer.putByte(0x40);
    writer.putChar('A');
    writer.putUint16(1);
    writer.putUint32(2);
    writer.putInt16(-3);
    writer.putInt32(-4);
    writer.putInt64(-5);
    writer.putFloat(1.0f);

    // then
    auto output = stream->str();
    BOOST_CHECK_EQUAL(string("\x40\x41\x00\x01\x00\x00\x00\x02\xff\xfd\xff\xff\xff\xfc\xff\xff\xff\xff\xff\xff\xff\xfb\x3f\x80\x00\x00", 26), output);
}

BOOST_AUTO_TEST_SUITE_END()
