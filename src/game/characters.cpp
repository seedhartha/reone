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

#include "characters.h"

#include "../core/random.h"
#include "../resource/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CreatureConfiguration randomCharacter(Gender gender, ClassType clazz) {
    vector<Portrait> portraits;
    shared_ptr<TwoDaTable> table(Resources.find2DA("portraits"));
    int sex = gender == Gender::Female ? 1 : 0;

    for (auto &row : table->rows()) {
        if (row.getInt("forpc") == 1 && row.getInt("sex") == sex) {
            string resRef(row.getString("baseresref"));
            int appearanceNumber = row.getInt("appearancenumber");
            int appearanceS = row.getInt("appearance_s");
            int appearanceL = row.getInt("appearance_l");

            Portrait portrait;
            portrait.resRef = move(resRef);
            portrait.appearanceNumber = appearanceNumber;
            portrait.appearanceS = appearanceS;
            portrait.appearanceL = appearanceL;

            portraits.push_back(move(portrait));
        }
    }

    int portraitIdx = random(0, static_cast<int>(portraits.size()) - 1);
    const Portrait &portrait = portraits[portraitIdx];
    int appearance = 0;

    switch (clazz) {
        case ClassType::Scoundrel:
        case ClassType::JediConsular:
            appearance = portrait.appearanceS;
            break;
        case ClassType::Soldier:
        case ClassType::JediGuardian:
            appearance = portrait.appearanceL;
            break;
        default:
            appearance = portrait.appearanceNumber;
            break;
    }

    CreatureConfiguration config;
    config.gender = gender;
    config.clazz = clazz;
    config.appearance = appearance;
    config.equipment.push_back("g_a_clothes01");

    return move(config);
}

} // namespace game

} // namespace reone
