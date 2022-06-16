/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#pragma once

#include "../../../src/audio/format/mp3reader.h"

namespace reone {

namespace audio {

class MockMp3Reader : public Mp3Reader {
public:
    void load(IInputStream &stream) override {
        _loadInvocations.push_back(std::make_tuple(&stream));
    }

    const std::vector<std::tuple<IInputStream *>> &loadInvocations() const {
        return _loadInvocations;
    }

private:
    std::vector<std::tuple<IInputStream *>> _loadInvocations;
};

class MockMp3ReaderFactory : public IMp3ReaderFactory {
public:
    MockMp3ReaderFactory() :
        _instance(std::make_shared<MockMp3Reader>()) {
    }

    MockMp3ReaderFactory(std::shared_ptr<Mp3Reader> instance) :
        _instance(std::move(instance)) {
    }

    std::shared_ptr<Mp3Reader> create() override {
        return _instance;
    }

private:
    std::shared_ptr<Mp3Reader> _instance;
};

} // namespace audio

} // namespace reone
