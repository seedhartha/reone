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

/** @file
 *  Tests for MediaStream class.
 */

#include <boost/test/unit_test.hpp>

#include "../../src/engine/common/mediastream.h"

using namespace std;

using namespace reone;

class TestStream : public MediaStream<int> {
public:
    void addSourceFrame(int id) {
        _sourceFrames.push(make_shared<int>(id));
    }

    int ignoredTotal() const { return _ignoredTotal; }
    int fetchedTotal() const { return _fetchedTotal; }

private:
    queue<shared_ptr<int>> _sourceFrames;
    int _ignoredTotal { 0 };
    int _fetchedTotal { 0 };

    void ignoreFrames(int count) {
        for (int i = 0; i < count; ++i) {
            if (_sourceFrames.empty()) break;
            _sourceFrames.pop();
            ++_ignoredTotal;
        }
    }

    void fetchFrames(int count) {
        for (int i = 0; i < count; ++i) {
            if (_sourceFrames.empty()) break;
            _frames.push_back(_sourceFrames.front());
            _sourceFrames.pop();
            ++_fetchedTotal;
        }
    }
};

BOOST_AUTO_TEST_CASE(MediaStream_GetFrameFromStart) {
    TestStream stream;
    for (int i = 0; i <= stream.kBufferSize; ++i) {
        stream.addSourceFrame(i);
    }

    auto frame = stream.get(0);

    BOOST_TEST((*frame == 0));
    BOOST_TEST((stream.ignoredTotal() == 0));
    BOOST_TEST((stream.fetchedTotal() == stream.kBufferSize));
}

BOOST_AUTO_TEST_CASE(MediaStream_GetFrameFromMid) {
    TestStream stream;
    for (int i = 0; i <= stream.kBufferSize; ++i) {
        stream.addSourceFrame(i);
    }

    auto frame = stream.get(4);

    BOOST_TEST((*frame == 4));
    BOOST_TEST((stream.ignoredTotal() == 4));
    BOOST_TEST((stream.fetchedTotal() == stream.kBufferSize - 3));
}

BOOST_AUTO_TEST_CASE(MediaStream_GetFrameFromEnd) {
    TestStream stream;
    for (int i = 0; i <= stream.kBufferSize; ++i) {
        stream.addSourceFrame(i);
    }

    auto frame = stream.get(stream.kBufferSize);

    BOOST_TEST((*frame == stream.kBufferSize));
    BOOST_TEST((stream.ignoredTotal() == stream.kBufferSize));
    BOOST_TEST((stream.fetchedTotal() == 1));
}
