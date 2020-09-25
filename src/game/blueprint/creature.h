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

#include <string>
#include <unordered_map>
#include <vector>

#include "../../resources/gfffile.h"

namespace reone {

namespace game {

class CreatureBlueprint {
public:
    CreatureBlueprint() = default;

    void load(const std::string &resRef, const resources::GffStruct &utc);

    const std::string &tag() const;
    const std::vector<std::string> &equipment() const;
    int appearance() const;
    const std::string &conversation() const;

private:
    enum class ScriptType {
        OnNotice,
        SpellAt,
        Attacked,
        Damaged,
        Disturbed,
        EndRound,
        EndDialogu,
        Dialogue,
        Spawn,
        Rested,
        Death,
        UserDefine,
        OnBlocked
    };

    std::string _tag;
    std::vector<std::string> _equipment;
    int _appearance { 0 };
    std::string _conversation;
    std::unordered_map<ScriptType, std::string> _scripts;

    CreatureBlueprint(const CreatureBlueprint &) = delete;
    CreatureBlueprint &operator=(const CreatureBlueprint &) = delete;
};

} // namespace game

} // namespace reone
