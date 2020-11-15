/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <map>
#include <utility>
#include <vector>

namespace reone {

namespace game {

enum class Gender {
    Male = 0,
    Female = 1,
    Both = 2,
    Other = 3,
    None = 4
};

enum class ClassType {
    Soldier = 0,
    Scout = 1,
    Scoundrel = 2,
    JediGuardian = 3,
    JediConsular = 4,
    JediSentinel = 5,
    CombatDroid = 6,
    ExpertDroid = 7,
    Minion = 8,
    TechSpecialist = 9,
    BountyHunter = 10,
    JediWeaponMaster = 11,
    JediMaster = 12,
    JediWatchman = 13,
    SithMarauder = 14,
    SithLord = 15,
    SithAssassin = 16,

    Invalid = 255
};

enum class Ability {
    Strength = 0,
    Dexterity = 1,
    Constitution = 2,
    Intelligence = 3,
    Wisdom = 4,
    Charisma = 5
};

enum class Skill {
    ComputerUse = 0,
    Demolitions = 1,
    Stealth = 2,
    Awareness = 3,
    Persuade = 4,
    Repair = 5,
    Security = 6,
    TreatInjury = 7
};

struct CreatureAttributes {
    std::vector<std::pair<ClassType, int>> classLevels;
    int hitDice { 0 };
    std::map<Ability, int> abilities;
    std::map<Skill, int> skills;

    void computeHitDice() {
        hitDice = 0;
        for (auto &classLevel : classLevels) {
            hitDice += classLevel.second;
        }
    }

    ClassType getClassByPosition(int position) const {
        return (position - 1) < static_cast<int>(classLevels.size()) ?
            classLevels[static_cast<size_t>(position) - 1].first :
            ClassType::Invalid;
    }

    int getClassLevel(ClassType clazz) const {
        auto maybeClassLevel = find_if(
            classLevels.begin(), classLevels.end(),
            [&clazz](auto &classLevel) { return classLevel.first == clazz; });

        return maybeClassLevel != classLevels.end() ? maybeClassLevel->second : 0;
    }

    int getHitDice() const { return hitDice; }

    int strength() const { return abilities.find(Ability::Strength)->second; }
    int dexterity() const { return abilities.find(Ability::Dexterity)->second; }
    int constitution() const { return abilities.find(Ability::Constitution)->second; }
    int intelligence() const { return abilities.find(Ability::Intelligence)->second; }
    int wisdom() const { return abilities.find(Ability::Wisdom)->second; }
    int charisma() const { return abilities.find(Ability::Charisma)->second; }

    bool hasSkill(Skill skill) const {
        return skills.find(skill)->second > 0;
    }

    int computerUse() const { return skills.find(Skill::ComputerUse)->second; }
    int demolitions() const { return skills.find(Skill::Demolitions)->second; }
    int stealth() const { return skills.find(Skill::Stealth)->second; }
    int awareness() const { return skills.find(Skill::Awareness)->second; }
    int persuade() const { return skills.find(Skill::Persuade)->second; }
    int repair() const { return skills.find(Skill::Repair)->second; }
    int security() const { return skills.find(Skill::Security)->second; }
    int treatInjury() const { return skills.find(Skill::TreatInjury)->second; }
};

} // namespace game

} // namespace reone
