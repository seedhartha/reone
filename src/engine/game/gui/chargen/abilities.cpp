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

    _binding.btnAccept->setOnClick([this]() {
        if (_points > 0) return;
        updateCharacter();
        _charGen->goToNextStep();
        _charGen->openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen->openSteps();
    });
    _binding.btnRecommended->setOnClick([this]() {
        ClassType classType = _charGen->character().attributes.getEffectiveClass();
        shared_ptr<CreatureClass> clazz(_game->classes().get(classType));
        _attributes = clazz->defaultAttributes();
        _points = 0;
        refreshControls();
    });

    _binding.strLbl->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Strength, focus);
    });
    _binding.dexLbl->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Dexterity, focus);
    });
    _binding.conLbl->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Constitution, focus);
    });
    _binding.intLbl->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Intelligence, focus);
    });
    _binding.wisLbl->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Wisdom, focus);
    });
    _binding.chaLbl->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Charisma, focus);
    });

    _binding.strMinusBtn->setOnClick([this]() {
        onMinusButtonClick(Ability::Strength);
    });
    _binding.dexMinusBtn->setOnClick([this]() {
        onMinusButtonClick(Ability::Dexterity);
    });
    _binding.conMinusBtn->setOnClick([this]() {
        onMinusButtonClick(Ability::Constitution);
    });
    _binding.intMinusBtn->setOnClick([this]() {
        onMinusButtonClick(Ability::Intelligence);
    });
    _binding.wisMinusBtn->setOnClick([this]() {
        onMinusButtonClick(Ability::Wisdom);
    });
    _binding.chaMinusBtn->setOnClick([this]() {
        onMinusButtonClick(Ability::Charisma);
    });

    _binding.strPlusBtn->setOnClick([this]() {
        onPlusButtonClick(Ability::Strength);
    });
    _binding.dexPlusBtn->setOnClick([this]() {
        onPlusButtonClick(Ability::Dexterity);
    });
    _binding.conPlusBtn->setOnClick([this]() {
        onPlusButtonClick(Ability::Constitution);
    });
    _binding.intPlusBtn->setOnClick([this]() {
        onPlusButtonClick(Ability::Intelligence);
    });
    _binding.wisPlusBtn->setOnClick([this]() {
        onPlusButtonClick(Ability::Wisdom);
    });
    _binding.chaPlusBtn->setOnClick([this]() {
        onPlusButtonClick(Ability::Charisma);
    });
}

void CharGenAbilities::bindControls() {
    _binding.btnAccept = getControl<Button>("BTN_ACCEPT");
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnRecommended = getControl<Button>("BTN_RECOMMENDED");
    _binding.costPointsLbl = getControl<Label>("COST_POINTS_LBL");
    _binding.lbDesc = getControl<ListBox>("LB_DESC");
    _binding.remainingSelectionsLbl = getControl<Label>("REMAINING_SELECTIONS_LBL");

    _binding.strLbl = getControl<Label>("STR_LBL");
    _binding.dexLbl = getControl<Label>("DEX_LBL");
    _binding.conLbl = getControl<Label>("CON_LBL");
    _binding.intLbl = getControl<Label>("INT_LBL");
    _binding.wisLbl = getControl<Label>("WIS_LBL");
    _binding.chaLbl = getControl<Label>("CHA_LBL");

    _binding.strPointsBtn = getControl<Button>("STR_POINTS_BTN");
    _binding.dexPointsBtn = getControl<Button>("DEX_POINTS_BTN");
    _binding.conPointsBtn = getControl<Button>("CON_POINTS_BTN");
    _binding.intPointsBtn = getControl<Button>("INT_POINTS_BTN");
    _binding.wisPointsBtn = getControl<Button>("WIS_POINTS_BTN");
    _binding.chaPointsBtn = getControl<Button>("CHA_POINTS_BTN");

    _binding.strMinusBtn = getControl<Button>("STR_MINUS_BTN");
    _binding.dexMinusBtn = getControl<Button>("DEX_MINUS_BTN");
    _binding.conMinusBtn = getControl<Button>("CON_MINUS_BTN");
    _binding.intMinusBtn = getControl<Button>("INT_MINUS_BTN");
    _binding.wisMinusBtn = getControl<Button>("WIS_MINUS_BTN");
    _binding.chaMinusBtn = getControl<Button>("CHA_MINUS_BTN");

    _binding.strPlusBtn = getControl<Button>("STR_PLUS_BTN");
    _binding.dexPlusBtn = getControl<Button>("DEX_PLUS_BTN");
    _binding.conPlusBtn = getControl<Button>("CON_PLUS_BTN");
    _binding.intPlusBtn = getControl<Button>("INT_PLUS_BTN");
    _binding.wisPlusBtn = getControl<Button>("WIS_PLUS_BTN");
    _binding.chaPlusBtn = getControl<Button>("CHA_PLUS_BTN");

    if (_game->isKotOR()) {
        _binding.lblAbilityMod = getControl<Label>("LBL_ABILITY_MOD");
    }
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
    if (_game->isKotOR()) {
        _binding.lblAbilityMod->setTextMessage("");
    }

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

void CharGenAbilities::updateCharacter() {
    Character character(_charGen->character());
    for (auto &abilityScore : _attributes.abilityScores()) {
        character.attributes.setAbilityScore(abilityScore.first, abilityScore.second);
    }
    _charGen->setCharacter(move(character));
}

void CharGenAbilities::onAbilityLabelFocusChanged(Ability ability, bool focus) {
    if (!focus) return;

    auto maybeDescription = g_descStrRefByAbility.find(ability);
    if (maybeDescription == g_descStrRefByAbility.end()) return;

    string description(_game->strings().get(maybeDescription->second));
    _binding.lbDesc->clearItems();
    _binding.lbDesc->addTextLinesAsItems(description);
}

void CharGenAbilities::onMinusButtonClick(Ability ability) {
    _attributes.setAbilityScore(ability, _attributes.getAbilityScore(ability) - 1);
    _points += getPointCost(ability);
    refreshControls();
}

void CharGenAbilities::onPlusButtonClick(Ability ability) {
    _points -= getPointCost(ability);
    _attributes.setAbilityScore(ability, _attributes.getAbilityScore(ability) + 1);
    refreshControls();
}

} // namespace game

} // namespace reone
