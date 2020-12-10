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

#include "classutil.h"

#include <map>

#include "../../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

static map<ClassType, int> g_classStrRefs {
    { ClassType::Scout, 133 },
    { ClassType::Soldier, 134 },
    { ClassType::Scoundrel, 135 },
    { ClassType::JediGuardian, 353 },
    { ClassType::JediConsular, 354 },
    { ClassType::JediSentinel, 355 }
};

string getClassTitle(ClassType clazz) {
    int strRef = g_classStrRefs.find(clazz)->second;
    return Resources::instance().getString(strRef);
}

CreatureAttributes getClassAttributes(ClassType clazz) {
    shared_ptr<TwoDaTable> classes(Resources::instance().get2DA("classes"));
    int row = static_cast<int>(clazz);

    CreatureAttributes attrs;
    attrs.setAbilityScore(Ability::Strength, classes->getInt(row, "str"));
    attrs.setAbilityScore(Ability::Dexterity, classes->getInt(row, "dex"));
    attrs.setAbilityScore(Ability::Constitution, classes->getInt(row, "con"));
    attrs.setAbilityScore(Ability::Intelligence, classes->getInt(row, "int"));
    attrs.setAbilityScore(Ability::Wisdom, classes->getInt(row, "wis"));
    attrs.setAbilityScore(Ability::Charisma, classes->getInt(row, "cha"));

    return move(attrs);
}

int getClassHitPoints(ClassType clazz, int level) {
    shared_ptr<TwoDaTable> classes(Resources::instance().get2DA("classes"));
    int row = static_cast<int>(clazz);

    return level * classes->getInt(row, "hitdie");
}

int getClassDefenseBonus(ClassType clazz, int level) {
    switch (clazz) {
        case ClassType::JediConsular:
        case ClassType::JediGuardian:
        case ClassType::JediSentinel:
        case ClassType::Scoundrel:
            return 2 + (2 * (level / 6));
        default:
            return 0;
    }
}

} // namespace game

} // namespace reone
