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

#include "portrait.h"
#include "portraits.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static vector<Portrait> getPCPortraitsByGender(Gender gender) {
    vector<Portrait> result;
    int sex = gender == Gender::Female ? 1 : 0;
    for (auto &portrait : Portraits::instance().portraits()) {
        if (portrait.forPC && portrait.sex == sex) {
            result.push_back(portrait);
        }
    }
    return move(result);
}

int getRandomCharacterAppearance(Gender gender, ClassType clazz) {
    int result = 0;
    vector<Portrait> portraits(getPCPortraitsByGender(gender));
    int portraitIdx = random(0, static_cast<int>(portraits.size()) - 1);
    const Portrait &portrait = portraits[portraitIdx];

    switch (clazz) {
        case ClassType::Scoundrel:
        case ClassType::JediConsular:
            result = portrait.appearanceS;
            break;
        case ClassType::Soldier:
        case ClassType::JediGuardian:
            result = portrait.appearanceL;
            break;
        default:
            result = portrait.appearanceNumber;
            break;
    }

    return result;
}

} // namespace game

} // namespace reone
