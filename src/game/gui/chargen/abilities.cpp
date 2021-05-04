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

#include "../../../gui/control/listbox.h"
#include "../../../resource/strings.h"

#include "../../d20/classes.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
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

static const unordered_map<Ability, int> g_descStrRefByAbility {
    { Ability::Strength, 222 },
    { Ability::Dexterity, 223 },
    { Ability::Constitution, 224 },
    { Ability::Intelligence, 226  },
    { Ability::Wisdom, 225  },
    { Ability::Charisma, 227 }
};

CharGenAbilities::CharGenAbilities(CharacterGeneration *charGen, GameID gameId, const GraphicsOptions &opts) :
    GameGUI(gameId, opts),
    _charGen(charGen) {

    _resRef = getResRef("abchrgen");

    initForGame();
}

void CharGenAbilities::load() {
    GUI::load();

    static vector<string> labels { "STR_LBL", "DEX_LBL", "CON_LBL", "INT_LBL", "WIS_LBL", "CHA_LBL" };
    for (auto &label : labels) {
        configureControl(label, [this](Control &control) {
            control.setFocusable(true);
            control.setHilightColor(getBaseColor(_gameId));
        });
    }

    ListBox &lbDesc = getControl<ListBox>("LB_DESC");
    lbDesc.setProtoMatchContent(true);

    disableControl("STR_POINTS_BTN");
    disableControl("DEX_POINTS_BTN");
    disableControl("CON_POINTS_BTN");
    disableControl("INT_POINTS_BTN");
    disableControl("WIS_POINTS_BTN");
    disableControl("CHA_POINTS_BTN");
}

void CharGenAbilities::reset(bool newGame) {
    _abilities = _charGen->character().attributes.abilities();
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

    } else if (control == "BTN_RECOMMENDED") {
        ClassType classType = _charGen->character().attributes.getEffectiveClass();
        shared_ptr<CreatureClass> clazz(Classes::instance().get(classType));
        _abilities = clazz->defaultAttributes().abilities();
        _points = 0;
        refreshControls();

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
    Character character(_charGen->character());
    character.attributes.setAbilities(_abilities);
    _charGen->setCharacter(move(character));
}

void CharGenAbilities::onFocusChanged(const string &control, bool focus) {
    if (focus && control.size() == 7ll && boost::ends_with(control, "_LBL")) {
        string alias(control.substr(0, 3));
        auto maybeAbility = g_abilityByAlias.find(alias);
        if (maybeAbility != g_abilityByAlias.end()) {
            auto maybeDescription = g_descStrRefByAbility.find(maybeAbility->second);
            if (maybeDescription != g_descStrRefByAbility.end()) {
                string description(Strings::instance().get(maybeDescription->second));
                ListBox &listBox = getControl<ListBox>("LB_DESC");
                listBox.clearItems();
                listBox.addTextLinesAsItems(description);
            }
        }
    }
}

} // namespace game

} // namespace reone
