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

#include "reone/game/d20/skills.h"

#include "reone/resource/2da.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/strings.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Skills::init() {
    std::shared_ptr<TwoDA> skills(_twoDas.get("skills"));
    if (!skills) {
        return;
    }

    for (int row = 0; row < skills->getRowCount(); ++row) {
        std::string name(_strings.getText(skills->getInt(row, "name", -1)));
        std::string description(_strings.getText(skills->getInt(row, "description", -1)));
        std::shared_ptr<Texture> icon(_textures.get(skills->getString(row, "icon"), TextureUsage::GUI));

        auto skill = std::make_shared<Skill>();
        skill->name = std::move(name);
        skill->description = std::move(description);
        skill->icon = std::move(icon);
        _skills.insert(std::make_pair(static_cast<SkillType>(row), std::move(skill)));
    }
}

std::shared_ptr<Skill> Skills::get(SkillType type) const {
    auto it = _skills.find(type);
    return it != _skills.end() ? it->second : nullptr;
}

} // namespace game

} // namespace reone
