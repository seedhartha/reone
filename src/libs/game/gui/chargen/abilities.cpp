/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/gui/chargen/abilities.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/resource/strings.h"

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStartingPoints = 30;
static constexpr int kMinAbilityScore = 8;
static constexpr int kMaxAbilityScore = 18;

static const std::unordered_map<std::string, Ability> g_abilityByAlias {
    {"STR", Ability::Strength},
    {"DEX", Ability::Dexterity},
    {"CON", Ability::Constitution},
    {"INT", Ability::Intelligence},
    {"WIS", Ability::Wisdom},
    {"CHA", Ability::Charisma}};

static const std::unordered_map<Ability, int> g_descStrRefByAbility {
    {Ability::Strength, 222},
    {Ability::Dexterity, 223},
    {Ability::Constitution, 224},
    {Ability::Intelligence, 226},
    {Ability::Wisdom, 225},
    {Ability::Charisma, 227}};

void CharGenAbilities::onGUILoaded() {
    bindControls();

    _binding.lbDesc->setProtoMatchContent(true);

    std::vector<Label *> labels {
        _binding.strLbl.get(),
        _binding.dexLbl.get(),
        _binding.conLbl.get(),
        _binding.intLbl.get(),
        _binding.wisLbl.get(),
        _binding.chaLbl.get()};
    for (auto &label : labels) {
        label->setFocusable(true);
        label->setHilightColor(_baseColor);
    }

    _binding.strPointsBtn->setDisabled(true);
    _binding.dexPointsBtn->setDisabled(true);
    _binding.conPointsBtn->setDisabled(true);
    _binding.intPointsBtn->setDisabled(true);
    _binding.wisPointsBtn->setDisabled(true);
    _binding.chaPointsBtn->setDisabled(true);

    _binding.btnAccept->setOnClick([this]() {
        if (_points > 0)
            return;
        updateCharacter();
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen.openSteps();
    });
    _binding.btnRecommended->setOnClick([this]() {
        ClassType classType = _charGen.character().attributes.getEffectiveClass();
        std::shared_ptr<CreatureClass> clazz(_services.game.classes.get(classType));
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
    _binding.btnAccept = findControl<Button>("BTN_ACCEPT");
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnRecommended = findControl<Button>("BTN_RECOMMENDED");
    _binding.costPointsLbl = findControl<Label>("COST_POINTS_LBL");
    _binding.lbDesc = findControl<ListBox>("LB_DESC");
    _binding.remainingSelectionsLbl = findControl<Label>("REMAINING_SELECTIONS_LBL");

    _binding.strLbl = findControl<Label>("STR_LBL");
    _binding.dexLbl = findControl<Label>("DEX_LBL");
    _binding.conLbl = findControl<Label>("CON_LBL");
    _binding.intLbl = findControl<Label>("INT_LBL");
    _binding.wisLbl = findControl<Label>("WIS_LBL");
    _binding.chaLbl = findControl<Label>("CHA_LBL");

    _binding.strPointsBtn = findControl<Button>("STR_POINTS_BTN");
    _binding.dexPointsBtn = findControl<Button>("DEX_POINTS_BTN");
    _binding.conPointsBtn = findControl<Button>("CON_POINTS_BTN");
    _binding.intPointsBtn = findControl<Button>("INT_POINTS_BTN");
    _binding.wisPointsBtn = findControl<Button>("WIS_POINTS_BTN");
    _binding.chaPointsBtn = findControl<Button>("CHA_POINTS_BTN");

    _binding.strMinusBtn = findControl<Button>("STR_MINUS_BTN");
    _binding.dexMinusBtn = findControl<Button>("DEX_MINUS_BTN");
    _binding.conMinusBtn = findControl<Button>("CON_MINUS_BTN");
    _binding.intMinusBtn = findControl<Button>("INT_MINUS_BTN");
    _binding.wisMinusBtn = findControl<Button>("WIS_MINUS_BTN");
    _binding.chaMinusBtn = findControl<Button>("CHA_MINUS_BTN");

    _binding.strPlusBtn = findControl<Button>("STR_PLUS_BTN");
    _binding.dexPlusBtn = findControl<Button>("DEX_PLUS_BTN");
    _binding.conPlusBtn = findControl<Button>("CON_PLUS_BTN");
    _binding.intPlusBtn = findControl<Button>("INT_PLUS_BTN");
    _binding.wisPlusBtn = findControl<Button>("WIS_PLUS_BTN");
    _binding.chaPlusBtn = findControl<Button>("CHA_PLUS_BTN");

    if (!_game.isTSL()) {
        _binding.lblAbilityMod = findControl<Label>("LBL_ABILITY_MOD");
    }
}

void CharGenAbilities::reset(bool newGame) {
    _attributes = _charGen.character().attributes;
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
    _binding.remainingSelectionsLbl->setTextMessage(std::to_string(_points));
    _binding.costPointsLbl->setTextMessage("");
    if (!_game.isTSL()) {
        _binding.lblAbilityMod->setTextMessage("");
    }

    _binding.strPointsBtn->setTextMessage(std::to_string(_attributes.strength()));
    _binding.dexPointsBtn->setTextMessage(std::to_string(_attributes.dexterity()));
    _binding.conPointsBtn->setTextMessage(std::to_string(_attributes.constitution()));
    _binding.intPointsBtn->setTextMessage(std::to_string(_attributes.intelligence()));
    _binding.wisPointsBtn->setTextMessage(std::to_string(_attributes.wisdom()));
    _binding.chaPointsBtn->setTextMessage(std::to_string(_attributes.charisma()));

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

static Ability getAbilityByAlias(const std::string &alias) {
    return g_abilityByAlias.find(alias)->second;
}

void CharGenAbilities::updateCharacter() {
    Character character(_charGen.character());
    for (auto &abilityScore : _attributes.abilityScores()) {
        character.attributes.setAbilityScore(abilityScore.first, abilityScore.second);
    }
    _charGen.setCharacter(std::move(character));
}

void CharGenAbilities::onAbilityLabelFocusChanged(Ability ability, bool focus) {
    if (!focus)
        return;

    auto maybeDescription = g_descStrRefByAbility.find(ability);
    if (maybeDescription == g_descStrRefByAbility.end())
        return;

    std::string description(_services.resource.strings.getText(maybeDescription->second));
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
