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

#include "characterutil.h"

#include "../common/random.h"
#include "../resource/resources.h"

#include "blueprint/creature.h"
#include "d20/classes.h"
#include "portrait.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

unique_ptr<StaticCreatureBlueprint> randomCharacter(Gender gender, ClassType clazz) {
    vector<Portrait> portraits;
    shared_ptr<TwoDA> twoDa(Resources::instance().get2DA("portraits"));
    int sex = gender == Gender::Female ? 1 : 0;

    for (int row = 0; row < twoDa->getRowCount(); ++row) {
        if (twoDa->getInt(row, "forpc") == 1 && twoDa->getInt(row, "sex") == sex) {
            string resRef(twoDa->getString(row, "baseresref"));
            int appearanceNumber = twoDa->getInt(row, "appearancenumber");
            int appearanceS = twoDa->getInt(row, "appearance_s");
            int appearanceL = twoDa->getInt(row, "appearance_l");

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

    shared_ptr<CreatureClass> creatureClass = Classes::instance().get(clazz);

    auto character = make_unique<StaticCreatureBlueprint>();
    character->setGender(gender);
    character->setAppearance(appearance);
    character->setAttributes(creatureClass->defaultAttributes());
    character->addEquippedItem("g_a_clothes01");

    return move(character);
}

} // namespace game

} // namespace reone
