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

#include "reone/audio/di/services.h"
#include "reone/graphics/di/services.h"

#include "movie.h"

namespace reone {

namespace movie {

class IMovies {
public:
    virtual ~IMovies() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<IMovie> get(const std::string &name) = 0;
};

class Movies : public IMovies, boost::noncopyable {
public:
    Movies(std::filesystem::path gamePath,
           graphics::GraphicsServices &graphicsSvc,
           audio::AudioServices &audioSvc) :
        _gamePath(gamePath),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc) {
    }

    void clear() override {
        _objects.clear();
    }

    std::shared_ptr<IMovie> get(const std::string &name) override {
        auto maybeObject = _objects.find(name);
        if (maybeObject != _objects.end()) {
            return maybeObject->second;
        }
        auto object = doGet(name);
        return _objects.insert(make_pair(name, std::move(object))).first->second;
    }

private:
    std::filesystem::path _gamePath;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;

    std::unordered_map<std::string, std::shared_ptr<IMovie>> _objects;

    std::shared_ptr<IMovie> doGet(std::string name);
};

} // namespace movie

} // namespace reone
