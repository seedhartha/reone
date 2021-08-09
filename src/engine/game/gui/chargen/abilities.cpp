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
    bindControls();

    _binding.lbDesc->setProtoMatchContent(true);

    vector<Label *> labels {
        _binding.strLbl.get(),
        _binding.dexLbl.get(),
        _binding.conLbl.get(),
        _binding.intLbl.get(),
        _binding.wisLbl.get(),
        _binding.chaLbl.get()
    };
    for (auto &label : labels) {
        label->setFocusable(true);
        label->setHilightColor(_game->getGUIColorBase());
    }

    _binding.strPointsBtn->setDisabled(true);
    _binding.dexPointsBtn->setDisabled(true);
    _binding.conPointsBtn->setDisabled(true);
    _binding.intPointsBtn->setDisabled(true);
    _binding.wisPointsBtn->setDisabled(true);
    _binding.chaPointsBtn->setDisabled(true);
}

void CharGenAbilities::bindControls() {
    _binding.lbDesc = getControlPtr<ListBox>("LB_DESC");
    _binding.remainingSelectionsLbl = getControlPtr<Label>("REMAINING_SELECTIONS_LBL");
    _binding.costPointsLbl = getControlPtr<Label>("COST_POINTS_LBL");
    _binding.lblAbilityMod = getControlPtr<Label>("LBL_ABILITY_MOD");

    _binding.strLbl = getControlPtr<Label>("STR_LBL");
    _binding.dexLbl = getControlPtr<Label>("DEX_LBL");
    _binding.conLbl = getControlPtr<Label>("CON_LBL");
    _binding.intLbl = getControlPtr<Label>("INT_LBL");
    _binding.wisLbl = getControlPtr<Label>("WIS_LBL");
    _binding.chaLbl = getControlPtr<Label>("CHA_LBL");

    _binding.strPointsBtn = getControlPtr<Button>("STR_POINTS_BTN");
    _binding.dexPointsBtn = getControlPtr<Button>("DEX_POINTS_BTN");
    _binding.conPointsBtn = getControlPtr<Button>("CON_POINTS_BTN");
    _binding.intPointsBtn = getControlPtr<Button>("INT_POINTS_BTN");
    _binding.wisPointsBtn = getControlPtr<Button>("WIS_POINTS_BTN");
    _binding.chaPointsBtn = getControlPtr<Button>("CHA_POINTS_BTN");

    _binding.strMinusBtn = getControlPtr<Button>("STR_MINUS_BTN");
    _binding.dexMinusBtn = getControlPtr<Button>("DEX_MINUS_BTN");
    _binding.conMinusBtn = getControlPtr<Button>("CON_MINUS_BTN");
    _binding.intMinusBtn = getControlPtr<Button>("INT_MINUS_BTN");
    _binding.wisMinusBtn = getControlPtr<Button>("WIS_MINUS_BTN");
    _binding.chaMinusBtn = getControlPtr<Button>("CHA_MINUS_BTN");

    _binding.strPlusBtn = getControlPtr<Button>("STR_PLUS_BTN");
    _binding.dexPlusBtn = getControlPtr<Button>("DEX_PLUS_BTN");
    _binding.conPlusBtn = getControlPtr<Button>("CON_PLUS_BTN");
    _binding.intPlusBtn = getControlPtr<Button>("INT_PLUS_BTN");
    _binding.wisPlusBtn = getControlPtr<Button>("WIS_PLUS_BTN");
    _binding.chaPlusBtn = getControlPtr<Button>("CHA_PLUS_BTN");
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
    _binding.remainingSelectionsLbl->setTextMessage(to_string(_points));
    _binding.costPointsLbl->setTextMessage("");
    _binding.lblAbilityMod->setTextMessage("");

    _binding.strPointsBtn->setTextMessage(to_string(_attributes.strength()));
    _binding.dexPointsBtn->setTextMessage(to_string(_attributes.dexterity()));
    _binding.conPointsBtn->setTextMessage(to_string(_attributes.constitution()));
    _binding.intPointsBtn->setTextMessage(to_string(_attributes.intelligence()));
    _binding.wisPointsBtn->setTextMessage(to_string(_attributes.wisdom()));
    _binding.chaPointsBtn->setTextMessage(to_string(_attributes.charisma()));

    _binding.strMinusBtn->setVisible(_attributes.strength() > kMinAbilityScore);
    _binding.dexMinusBtn->setVisible(_attributes.dexterity() > kMinAbilityScore);
    _binding.conMinusBtn->setVisible(_attributes.constitution() > kMinAbilityScore);
    _binding.intMinusBtn->setVisible(_attributes.intelligence() > kMinAbilityScore);
    _binding.wisMinusBtn->setVisible(_attributes.wisdom() > kMinAbilityScore);
    _binding.chaMinusBtn->setVisible(_attributes.charisma() > kMinAbilityScore);

    _binding.strPlusBtn->setVisible(_points >= getPointCost(Ability::Strength) && _attributes.strength() < kMaxAbilityScore);
    _binding.dexPlusBtn->setVisible(_points >= getPointCost(Ability::Dexterity) && _attributes.dexterity() < kMaxAbilityScore);
    _binding.conPlusBtn->setVisible(_points >= getPointCost(Ability::Constitution) && _attributes.constitution() < kMaxAbilityScore);
    _binding.intPlusBtn->setVisible(_points >= getPointCost(Ability::Intelligence) && _attributes.intelligence() < kMaxAbilityScore);
    _binding.wisPlusBtn->setVisible(_points >= getPointCost(Ability::Wisdom) && _attributes.wisdom() < kMaxAbilityScore);
    _binding.chaPlusBtn->setVisible(_points >= getPointCost(Ability::Charisma) && _attributes.charisma() < kMaxAbilityScore);
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
                _binding.lbDesc->clearItems();
                _binding.lbDesc->addTextLinesAsItems(description);
            }
        }
    }
}

} // namespace game

} // namespace reone
