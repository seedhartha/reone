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

constexpr int kStartingPoints = 30;
constexpr int kMinAbilityScore = 8;
constexpr int kMaxAbilityScore = 18;

static const unordered_map<std::string, Ability> g_abilityByAlias {
    { "STR", Ability::Strength },
    { "DEX", Ability::Dexterity },
    { "CON", Ability::Constitution },
    { "INT", Ability::Intelligence },
    { "WIS", Ability::Wisdom },
    { "CHA", Ability::Charisma }
};

CharGenAbilities::CharGenAbilities(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("abchrgen");

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
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

void CharGenAbilities::reset() {
    _attributes.setAbilityScore(Ability::Strength, 8);
    _attributes.setAbilityScore(Ability::Dexterity, 8);
    _attributes.setAbilityScore(Ability::Constitution, 8);
    _attributes.setAbilityScore(Ability::Intelligence, 8);
    _attributes.setAbilityScore(Ability::Wisdom, 8);
    _attributes.setAbilityScore(Ability::Charisma, 8);

    _points = kStartingPoints;

    refreshControls();
}

void CharGenAbilities::refreshControls() {
    setControlText("REMAINING_SELECTIONS_LBL", to_string(_points));
    setControlText("COST_POINTS_LBL", "");
    setControlText("LBL_ABILITY_MOD", "");

    setControlVisible("STR_MINUS_BTN", _attributes.strength() > kMinAbilityScore);
    setControlVisible("DEX_MINUS_BTN", _attributes.dexterity() > kMinAbilityScore);
    setControlVisible("CON_MINUS_BTN", _attributes.constitution() > kMinAbilityScore);
    setControlVisible("INT_MINUS_BTN", _attributes.intelligence() > kMinAbilityScore);
    setControlVisible("WIS_MINUS_BTN", _attributes.wisdom() > kMinAbilityScore);
    setControlVisible("CHA_MINUS_BTN", _attributes.charisma() > kMinAbilityScore);

    setControlText("STR_POINTS_BTN", to_string(_attributes.strength()));
    setControlText("DEX_POINTS_BTN", to_string(_attributes.dexterity()));
    setControlText("CON_POINTS_BTN", to_string(_attributes.constitution()));
    setControlText("INT_POINTS_BTN", to_string(_attributes.intelligence()));
    setControlText("WIS_POINTS_BTN", to_string(_attributes.wisdom()));
    setControlText("CHA_POINTS_BTN", to_string(_attributes.charisma()));

    setControlVisible("STR_PLUS_BTN", _points >= getPointCost(Ability::Strength) && _attributes.strength() < kMaxAbilityScore);
    setControlVisible("DEX_PLUS_BTN", _points >= getPointCost(Ability::Dexterity) && _attributes.dexterity() < kMaxAbilityScore);
    setControlVisible("CON_PLUS_BTN", _points >= getPointCost(Ability::Constitution) && _attributes.constitution() < kMaxAbilityScore);
    setControlVisible("INT_PLUS_BTN", _points >= getPointCost(Ability::Intelligence) && _attributes.intelligence() < kMaxAbilityScore);
    setControlVisible("WIS_PLUS_BTN", _points >= getPointCost(Ability::Wisdom) && _attributes.wisdom() < kMaxAbilityScore);
    setControlVisible("CHA_PLUS_BTN", _points >= getPointCost(Ability::Charisma) && _attributes.charisma() < kMaxAbilityScore);
}

int CharGenAbilities::getPointCost(Ability ability) const {
    return glm::clamp((_attributes.getAbilityScore(ability) - 10) / 2, 1, 3);
}

static Ability getAbilityByAlias(const string &alias) {
    return g_abilityByAlias.find(alias)->second;
}

void CharGenAbilities::onClick(const string &control) {
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
        _attributes.setAbilityScore(ability, _attributes.getAbilityScore(ability) - 1);
        _points += getPointCost(ability);
        refreshControls();
    } else if (boost::ends_with(control, "_PLUS_BTN")) {
        Ability ability = getAbilityByAlias(control.substr(0, 3));
        _points -= getPointCost(ability);
        _attributes.setAbilityScore(ability, _attributes.getAbilityScore(ability) + 1);
        refreshControls();
    }
}

void CharGenAbilities::updateCharacter() {
    StaticCreatureBlueprint blueprint(_charGen->character());
    blueprint.setAttributes(_attributes);
    _charGen->setCharacter(move(blueprint));
}

} // namespace game

} // namespace reone
