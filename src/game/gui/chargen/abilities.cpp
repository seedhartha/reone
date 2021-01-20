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

#include "abilities.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include "glm/common.hpp"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStartingPoints = 30;
static constexpr int kMinAbilityScore = 8;
static constexpr int kMaxAbilityScore = 18;

static const unordered_map<string, Ability> g_abilityByAlias {
    { "STR", Ability::Strength },
    { "DEX", Ability::Dexterity },
    { "CON", Ability::Constitution },
    { "INT", Ability::Intelligence },
    { "WIS", Ability::Wisdom },
    { "CHA", Ability::Charisma }
};

CharGenAbilities::CharGenAbilities(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GameGUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("abchrgen");

    initForGame();
}

void CharGenAbilities::load() {
    GUI::load();

    disableControl("STR_POINTS_BTN");
    disableControl("DEX_POINTS_BTN");
    disableControl("CON_POINTS_BTN");
    disableControl("INT_POINTS_BTN");
    disableControl("WIS_POINTS_BTN");
    disableControl("CHA_POINTS_BTN");

    disableControl("BTN_RECOMMENDED");
}

void CharGenAbilities::reset(bool newGame) {
    CreatureAttributes &attributes = _charGen->character().attributes();
    _abilities = attributes.abilities();
    _points = newGame ? kStartingPoints : 1;

    if (newGame) {
        _abilities.setScore(Ability::Strength, kMinAbilityScore);
        _abilities.setScore(Ability::Dexterity, kMinAbilityScore);
        _abilities.setScore(Ability::Constitution, kMinAbilityScore);
        _abilities.setScore(Ability::Intelligence, kMinAbilityScore);
        _abilities.setScore(Ability::Wisdom, kMinAbilityScore);
        _abilities.setScore(Ability::Charisma, kMinAbilityScore);
    }

    refreshControls();
}

void CharGenAbilities::refreshControls() {
    setControlText("REMAINING_SELECTIONS_LBL", to_string(_points));
    setControlText("COST_POINTS_LBL", "");
    setControlText("LBL_ABILITY_MOD", "");

    setControlVisible("STR_MINUS_BTN", _abilities.strength() > kMinAbilityScore);
    setControlVisible("DEX_MINUS_BTN", _abilities.dexterity() > kMinAbilityScore);
    setControlVisible("CON_MINUS_BTN", _abilities.constitution() > kMinAbilityScore);
    setControlVisible("INT_MINUS_BTN", _abilities.intelligence() > kMinAbilityScore);
    setControlVisible("WIS_MINUS_BTN", _abilities.wisdom() > kMinAbilityScore);
    setControlVisible("CHA_MINUS_BTN", _abilities.charisma() > kMinAbilityScore);

    setControlText("STR_POINTS_BTN", to_string(_abilities.strength()));
    setControlText("DEX_POINTS_BTN", to_string(_abilities.dexterity()));
    setControlText("CON_POINTS_BTN", to_string(_abilities.constitution()));
    setControlText("INT_POINTS_BTN", to_string(_abilities.intelligence()));
    setControlText("WIS_POINTS_BTN", to_string(_abilities.wisdom()));
    setControlText("CHA_POINTS_BTN", to_string(_abilities.charisma()));

    setControlVisible("STR_PLUS_BTN", _points >= getPointCost(Ability::Strength) && _abilities.strength() < kMaxAbilityScore);
    setControlVisible("DEX_PLUS_BTN", _points >= getPointCost(Ability::Dexterity) && _abilities.dexterity() < kMaxAbilityScore);
    setControlVisible("CON_PLUS_BTN", _points >= getPointCost(Ability::Constitution) && _abilities.constitution() < kMaxAbilityScore);
    setControlVisible("INT_PLUS_BTN", _points >= getPointCost(Ability::Intelligence) && _abilities.intelligence() < kMaxAbilityScore);
    setControlVisible("WIS_PLUS_BTN", _points >= getPointCost(Ability::Wisdom) && _abilities.wisdom() < kMaxAbilityScore);
    setControlVisible("CHA_PLUS_BTN", _points >= getPointCost(Ability::Charisma) && _abilities.charisma() < kMaxAbilityScore);
}

int CharGenAbilities::getPointCost(Ability ability) const {
    return glm::clamp(_abilities.getModifier(ability), 1, 3);
}

static Ability getAbilityByAlias(const string &alias) {
    return g_abilityByAlias.find(alias)->second;
}

void CharGenAbilities::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_ACCEPT") {
        if (_points == 0) {
            updateCharacter();
            _charGen->goToNextStep();
            _charGen->openSteps();
        }
    } else if (control == "BTN_BACK") {
        _charGen->openSteps();
    } else if (boost::ends_with(control, "_MINUS_BTN")) {
        Ability ability = getAbilityByAlias(control.substr(0, 3));
        _abilities.setScore(ability, _abilities.getScore(ability) - 1);
        _points += getPointCost(ability);
        refreshControls();
    } else if (boost::ends_with(control, "_PLUS_BTN")) {
        Ability ability = getAbilityByAlias(control.substr(0, 3));
        _points -= getPointCost(ability);
        _abilities.setScore(ability, _abilities.getScore(ability) + 1);
        refreshControls();
    }
}

void CharGenAbilities::updateCharacter() {
    _charGen->setAbilities(_abilities);
}

} // namespace game

} // namespace reone
