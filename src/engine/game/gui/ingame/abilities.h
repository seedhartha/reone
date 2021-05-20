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

#include "../gui.h"

#include "../../types.h"

namespace reone {

namespace game {

class AbilitiesMenu : public GameGUI {
public:
    AbilitiesMenu(Game *game);

    void load() override;

    void refreshControls();

private:
    struct SkillInfo {
        Skill skill;
        std::string name;
        std::string description;
        std::shared_ptr<graphics::Texture> icon;
    };

    std::unordered_map<Skill, SkillInfo> _skills;

    void onClick(const std::string &control) override;
    void onListBoxItemClick(const std::string &control, const std::string &item) override;

    void loadSkills();

    void refreshPortraits();

    std::shared_ptr<graphics::Texture> getFrameTexture(GameID gameId) const;
};

} // namespace game

} // namespace reone
