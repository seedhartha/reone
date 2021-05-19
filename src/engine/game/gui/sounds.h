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

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include "../../audio/stream.h"

namespace reone {

namespace game {

class GUISounds : boost::noncopyable {
public:
    GUISounds() = default;
    ~GUISounds();

    void init();
    void deinit();

    std::shared_ptr<audio::AudioStream> getOnClick() const { return _onClick; }
    std::shared_ptr<audio::AudioStream> getOnEnter() const { return _onEnter; }

private:
    std::shared_ptr<audio::AudioStream> _onClick;
    std::shared_ptr<audio::AudioStream> _onEnter;
};

} // namespace game

} // namespace reone
