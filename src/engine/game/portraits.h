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

#include <functional>
#include <string>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "../graphics/texture.h"

#include "portrait.h"

namespace reone {

namespace game {

class Portraits : boost::noncopyable {
public:
    static Portraits &instance();

    void init();

    std::shared_ptr<graphics::Texture> getTextureByIndex(int index);
    std::shared_ptr<graphics::Texture> getTextureByAppearance(int appearance);

    const std::vector<Portrait> &portraits() const { return _portraits; }

private:
    std::vector<Portrait> _portraits;
};

} // namespace game

} // namespace reone
