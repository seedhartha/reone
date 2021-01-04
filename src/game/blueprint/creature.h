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

#include <string>
#include <memory>
#include <vector>

#include "../../resource/gfffile.h"

#include "../rp/attributes.h"

#include "blueprint.h"

namespace reone {

namespace game {

class Creature;

/**
 * Creature blueprint that loads data from a UTC file.
 */
class CreatureBlueprint : public Blueprint<Creature> {
public:
    CreatureBlueprint(const std::string &resRef, const std::shared_ptr<resource::GffStruct> &utc);

    void load(Creature &creature) override;

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

/**
 * Creature blueprint that has its data set manually.
 */
class StaticCreatureBlueprint : public Blueprint<Creature> {
public:
    void load(Creature &creature) override;

    void clearEquipment();
    void addEquippedItem(const std::string &resRef);

    ClassType getClass() const;

    Gender gender() const;
    int appearance() const;
    const CreatureAttributes &attributes() const;

    void setGender(Gender gender);
    void setClass(ClassType clazz);
    void setAppearance(int appearance);
    void setAttributes(CreatureAttributes attributes);

private:
    Gender _gender { Gender::Male };
    ClassType _class { ClassType::Soldier };
    int _appearance { 0 };
    CreatureAttributes _attributes;
    std::vector<std::string> _equipment;
};

} // namespace game

} // namespace reone
