/*
 * Copyright © 2020 Vsevolod Kremianskii
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
#include <string>

#include "item.h"

namespace reone {

namespace game {

class BlueprintManager {
public:
    static BlueprintManager &instance();

    std::shared_ptr<ItemBlueprint> findItem(const std::string &resRef);

private:
    BlueprintManager() = default;

    BlueprintManager(const BlueprintManager &) = delete;
    BlueprintManager &operator=(const BlueprintManager &) = delete;
};

#define BlueprintMan game::BlueprintManager::instance()

} // namespace game

} // namespace reone
