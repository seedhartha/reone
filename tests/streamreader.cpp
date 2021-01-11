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

#define BOOST_TEST_MODULE streamreader

#include <sstream>

#include <boost/test/included/unit_test.hpp>

#include "../src/common/streamwriter.h"
#include "../src/common/streamreader.h"
#include "../src/common/endianutil.h"

using namespace std;

using namespace reone;

constexpr float TOL = 1E-6f;

void UnitTestPacket(Endianess en) {
    uint64_t A = 0xff00ee334466aa55;
    uint64_t B = 0x123;
    int32_t C = 0x00ff3344;
    int32_t D = 0xff000000;
    double E = 0.124385738472;
    double F = 10024.1324324738742;
    float G = 13.24323f;
    float H = 0.11111243f;
    uint8_t I = 0x01;

    StreamWriter writer(std::make_shared<stringstream>(), en);
    writer << A << B << C << D << E << F << G << H << I;

    uint64_t a, b;
    uint32_t c, d;
    double e, f;
    float g, h;
    uint8_t i;

    StreamReader reader(static_pointer_cast<stringstream>(writer.getStream()), en);
    reader >> a >> b >> c >> d >> e >> f >> g >> h >> i;

    BOOST_TEST((a == A));
    BOOST_TEST((b == B));
    BOOST_TEST((c == C));
    BOOST_TEST((d == D));
    BOOST_TEST((abs(e - E) < TOL));
    BOOST_TEST((abs(f - F) < TOL));
    BOOST_TEST((abs(g - G) < TOL));
    BOOST_TEST((abs(h - H) < TOL));
    BOOST_TEST((i == I));
}

BOOST_AUTO_TEST_CASE(test_packet_readwrite) {
    UnitTestPacket(Endianess::Big);
    UnitTestPacket(Endianess::Little);
}

BOOST_AUTO_TEST_CASE(test_get_little_endian) {
    shared_ptr<istringstream> stream(new istringstream(string("\x01" "\xe8\x03" "\xa0\x86\x01\x00" "\x00\xe4\x0b\x54\x02\x00\x00\x00" "\x60\x79\xfe\xff" "\x00\x00\x80\x3f" "abc\0defgh", 32)));
    StreamReader reader(stream);
    BOOST_TEST((reader.getByte() == 0x01));
    BOOST_TEST((reader.getUint16() == 1000u));
    BOOST_TEST((reader.getUint32() == 100000u));
    BOOST_TEST((reader.getUint64() == 10000000000u));
    BOOST_TEST((reader.getInt32() == -100000));
    BOOST_TEST((reader.getFloat() == 1.0f));
    BOOST_TEST((reader.getCString() == "abc"));
    BOOST_TEST((reader.getString(3) == "def"));
}

BOOST_AUTO_TEST_CASE(test_get_big_endian) {
    shared_ptr<istringstream> stream(new istringstream(string("\x03\xe8" "\x00\x01\x86\xa0" "\x00\x00\x00\x02\x54\x0b\xe4\x00" "\xff\xfe\x79\x60" "\x3f\x80\x00\x00", 22)));
    StreamReader reader(stream, Endianess::Big);
    BOOST_TEST((reader.getUint16() == 1000u));
    BOOST_TEST((reader.getUint32() == 100000u));
    BOOST_TEST((reader.getUint64() == 10000000000u));
    BOOST_TEST((reader.getInt32() == -100000));
    BOOST_TEST((reader.getFloat() == 1.0f));
}
