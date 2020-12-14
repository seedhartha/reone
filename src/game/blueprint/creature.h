/*
 * Copyright (c) 2020 The reone project contributors
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
#include <memory>

#include "../../resource/gfffile.h"

#include "../rp/attributes.h"

namespace reone {

namespace game {

class Creature;

class CreatureBlueprint {
public:
    CreatureBlueprint(const std::string &resRef, const std::shared_ptr<resource::GffStruct> &utc);

    void load(Creature &creature);

    const std::string &resRef() const;
    int getAppearanceFromUtc() const;

private:
    std::string _resRef;
    std::shared_ptr<resource::GffStruct> _utc;

    CreatureBlueprint(const CreatureBlueprint &) = delete;
    CreatureBlueprint &operator=(const CreatureBlueprint &) = delete;

    void loadName(Creature &creature);
    void loadAttributes(Creature &creature);
    void loadAbilities(CreatureAttributes &attributes);
    void loadSkills(CreatureAttributes &attributes);
    void loadScripts(Creature &creature);
    void loadItems(Creature &creature);
};

} // namespace game

} // namespace reone
