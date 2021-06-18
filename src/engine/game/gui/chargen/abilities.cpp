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

#include "../../../gui/control/listbox.h"
#include "../../../resource/strings.h"

#include "../../d20/classes.h"
#include "../../game.h"

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

CharGenAbilities::CharGenAbilities(CharacterGeneration *charGen, Game *game) :
    GameGUI(game),
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
            control.setHilightColor(_game->getGUIColorBase());
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
    _attributes = _charGen->character().attributes;
    _points = newGame ? kStartingPoints : 1;

    if (newGame) {
        _attributes.setAbilityScore(Ability::Strength, kMinAbilityScore);
        _attributes.setAbilityScore(Ability::Dexterity, kMinAbilityScore);
        _attributes.setAbilityScore(Ability::Constitution, kMinAbilityScore);
        _attributes.setAbilityScore(Ability::Intelligence, kMinAbilityScore);
        _attributes.setAbilityScore(Ability::Wisdom, kMinAbilityScore);
        _attributes.setAbilityScore(Ability::Charisma, kMinAbilityScore);
    }

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
    return glm::clamp(_attributes.getAbilityModifier(ability), 1, 3);
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
        shared_ptr<CreatureClass> clazz(_game->services().classes().get(classType));
        _attributes = clazz->defaultAttributes();
        _points = 0;
        refreshControls();

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
    Character character(_charGen->character());
    for (auto &abilityScore : _attributes.abilityScores()) {
        character.attributes.setAbilityScore(abilityScore.first, abilityScore.second);
    }
    _charGen->setCharacter(move(character));
}

void CharGenAbilities::onFocusChanged(const string &control, bool focus) {
    if (focus && control.size() == 7ll && boost::ends_with(control, "_LBL")) {
        string alias(control.substr(0, 3));
        auto maybeAbility = g_abilityByAlias.find(alias);
        if (maybeAbility != g_abilityByAlias.end()) {
            auto maybeDescription = g_descStrRefByAbility.find(maybeAbility->second);
            if (maybeDescription != g_descStrRefByAbility.end()) {
                string description(_game->services().resource().strings().get(maybeDescription->second));
                ListBox &listBox = getControl<ListBox>("LB_DESC");
                listBox.clearItems();
                listBox.addTextLinesAsItems(description);
            }
        }
    }
}

} // namespace game

} // namespace reone
