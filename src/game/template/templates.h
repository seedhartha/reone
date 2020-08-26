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

class TemplateManager {
public:
    static TemplateManager &instance();

    std::shared_ptr<ItemTemplate> findItem(const std::string &resRef);

private:
    TemplateManager() = default;

    TemplateManager(const TemplateManager &) = delete;
    TemplateManager &operator=(const TemplateManager &) = delete;
};

#define TemplateMan game::TemplateManager::instance()

} // namespace game

} // namespace reone
