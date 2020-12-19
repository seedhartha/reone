/*
 * Copyright (c) 2020 The reone project contributors
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

#define BOOST_TEST_MODULE network

#include <boost/test/included/unit_test.hpp>

#include <sstream>

#include "../src/mp/delta.h"
#include "../src/mp/objects.h"
#include "../src/common/streamreader.h"
#include "../src/common/streamwriter.h"
#include "../src/common/endianutil.h"

using namespace std;

using namespace reone;
using namespace reone::mp;

constexpr float TOL = 1E-6f;

BOOST_AUTO_TEST_CASE(test_delta_serialize) {
    CreatureStatus status0{};
    CreatureStatus status1{};
    CreatureStatus status2{};

    status1.x = 5.1f;
    status1.z = 3.2f;
    status1.heading = 0.5555555555;
    status1.animframe = 3;
    status1.faction = 5;

    status2.x = 5.5f;

    auto stream = std::make_shared<stringstream>();
    StreamWriter writer(stream, getEndianess());
    
    writeDelta<CreatureStatus>(writer, status1, status0);
    auto tmp1 = stream->str();
    stream->str(string());
    stream->clear(); // reset

    writeDelta<CreatureStatus>(writer, status2, status0);
    auto tmp2 = stream->str();

    BOOST_TEST_MESSAGE("status1 delta string size: " << tmp1.size());
    BOOST_TEST_MESSAGE("status2 delta string size: " << tmp2.size());

    BOOST_TEST((tmp1.size() > tmp2.size()));
    
    StreamReader reader(std::make_shared<stringstream>(tmp1 + tmp2));
    CreatureStatus res1{}, res2{};

    // read tmp1 first
    readDelta(reader, res1);
    readDelta(reader, res2);

    BOOST_TEST((abs(res1.x - status1.x) < TOL));
    BOOST_TEST((abs(res1.z - status1.z) < TOL));
    BOOST_TEST((abs(res1.heading - status1.heading) < TOL));
    BOOST_TEST((res1.animframe == status1.animframe));
    BOOST_TEST((res1.faction == status1.faction));
    BOOST_TEST((abs(res2.x - status2.x) < TOL));
}
