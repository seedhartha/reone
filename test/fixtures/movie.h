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

#include "reone/movie/di/services.h"

namespace reone {

namespace movie {

class MockMovies : public IMovies, boost::noncopyable {
public:
    // Overrides

    void invalidate() override {}

    std::shared_ptr<Movie> get(const std::string &name) override {
        return _getStubs.at(name);
    }

    // END Overrides

    // Stubs

    void whenGetThenReturn(std::string name, std::shared_ptr<Movie> movie) {
        _getStubs.insert(std::make_pair(name, movie));
    }

    // END Stubs

private:
    std::map<std::string, std::shared_ptr<Movie>> _getStubs;
};

class TestMovieModule : boost::noncopyable {
public:
    void init() {
        _movies = std::make_unique<MockMovies>();
        _services = std::make_unique<MovieServices>(*_movies);
    }

    MockMovies &movies() {
        return *_movies;
    }

    MovieServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockMovies> _movies;

    std::unique_ptr<MovieServices> _services;
};

} // namespace movie

} // namespace reone
