/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include <gmock/gmock.h>

#include "reone/movie/di/services.h"
#include "reone/movie/movie.h"

namespace reone {

namespace movie {

class MockMovie : public IMovie, boost::noncopyable {
public:
    MOCK_METHOD(void, update, (float dt), (override));
    MOCK_METHOD(void, render, (), (override));
    MOCK_METHOD(void, finish, (), (override));
    MOCK_METHOD(bool, isFinished, (), (const override));
};

class TestMovieModule : boost::noncopyable {
public:
    void init() {
        _services = std::make_unique<MovieServices>();
    }

    MovieServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MovieServices> _services;
};

} // namespace movie

} // namespace reone
