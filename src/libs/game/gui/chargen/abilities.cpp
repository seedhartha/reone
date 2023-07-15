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

    _controls.LB_DESC->setProtoMatchContent(true);

    std::vector<Label *> labels {
        _controls.STR_LBL.get(),
        _controls.DEX_LBL.get(),
        _controls.CON_LBL.get(),
        _controls.INT_LBL.get(),
        _controls.WIS_LBL.get(),
        _controls.CHA_LBL.get()};
    for (auto &label : labels) {
        label->setFocusable(true);
        label->setHilightColor(_baseColor);
    }

    _controls.STR_POINTS_BTN->setDisabled(true);
    _controls.DEX_POINTS_BTN->setDisabled(true);
    _controls.CON_POINTS_BTN->setDisabled(true);
    _controls.INT_POINTS_BTN->setDisabled(true);
    _controls.WIS_POINTS_BTN->setDisabled(true);
    _controls.CHA_POINTS_BTN->setDisabled(true);

    _controls.BTN_ACCEPT->setOnClick([this]() {
        if (_points > 0)
            return;
        updateCharacter();
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _controls.BTN_BACK->setOnClick([this]() {
        _charGen.openSteps();
    });
    _controls.BTN_RECOMMENDED->setOnClick([this]() {
        ClassType classType = _charGen.character().attributes.getEffectiveClass();
        std::shared_ptr<CreatureClass> clazz(_services.game.classes.get(classType));
        _attributes = clazz->defaultAttributes();
        _points = 0;
        refreshControls();
    });

    _controls.STR_LBL->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Strength, focus);
    });
    _controls.DEX_LBL->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Dexterity, focus);
    });
    _controls.CON_LBL->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Constitution, focus);
    });
    _controls.INT_LBL->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Intelligence, focus);
    });
    _controls.WIS_LBL->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Wisdom, focus);
    });
    _controls.CHA_LBL->setOnFocusChanged([this](bool focus) {
        onAbilityLabelFocusChanged(Ability::Charisma, focus);
    });

    _controls.STR_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(Ability::Strength);
    });
    _controls.DEX_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(Ability::Dexterity);
    });
    _controls.CON_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(Ability::Constitution);
    });
    _controls.INT_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(Ability::Intelligence);
    });
    _controls.WIS_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(Ability::Wisdom);
    });
    _controls.CHA_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(Ability::Charisma);
    });

    _controls.STR_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(Ability::Strength);
    });
    _controls.DEX_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(Ability::Dexterity);
    });
    _controls.CON_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(Ability::Constitution);
    });
    _controls.INT_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(Ability::Intelligence);
    });
    _controls.WIS_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(Ability::Wisdom);
    });
    _controls.CHA_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(Ability::Charisma);
    });
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
    _controls.REMAINING_SELECTIONS_LBL->setTextMessage(std::to_string(_points));
    _controls.COST_POINTS_LBL->setTextMessage("");
    if (!_game.isTSL()) {
        _controls.LBL_ABILITY_MOD->setTextMessage("");
    }

    _controls.STR_POINTS_BTN->setTextMessage(std::to_string(_attributes.strength()));
    _controls.DEX_POINTS_BTN->setTextMessage(std::to_string(_attributes.dexterity()));
    _controls.CON_POINTS_BTN->setTextMessage(std::to_string(_attributes.constitution()));
    _controls.INT_POINTS_BTN->setTextMessage(std::to_string(_attributes.intelligence()));
    _controls.WIS_POINTS_BTN->setTextMessage(std::to_string(_attributes.wisdom()));
    _controls.CHA_POINTS_BTN->setTextMessage(std::to_string(_attributes.charisma()));

    _controls.STR_MINUS_BTN->setVisible(_attributes.strength() > kMinAbilityScore);
    _controls.DEX_MINUS_BTN->setVisible(_attributes.dexterity() > kMinAbilityScore);
    _controls.CON_MINUS_BTN->setVisible(_attributes.constitution() > kMinAbilityScore);
    _controls.INT_MINUS_BTN->setVisible(_attributes.intelligence() > kMinAbilityScore);
    _controls.WIS_MINUS_BTN->setVisible(_attributes.wisdom() > kMinAbilityScore);
    _controls.CHA_MINUS_BTN->setVisible(_attributes.charisma() > kMinAbilityScore);

    _controls.STR_PLUS_BTN->setVisible(_points >= getPointCost(Ability::Strength) && _attributes.strength() < kMaxAbilityScore);
    _controls.DEX_PLUS_BTN->setVisible(_points >= getPointCost(Ability::Dexterity) && _attributes.dexterity() < kMaxAbilityScore);
    _controls.CON_PLUS_BTN->setVisible(_points >= getPointCost(Ability::Constitution) && _attributes.constitution() < kMaxAbilityScore);
    _controls.INT_PLUS_BTN->setVisible(_points >= getPointCost(Ability::Intelligence) && _attributes.intelligence() < kMaxAbilityScore);
    _controls.WIS_PLUS_BTN->setVisible(_points >= getPointCost(Ability::Wisdom) && _attributes.wisdom() < kMaxAbilityScore);
    _controls.CHA_PLUS_BTN->setVisible(_points >= getPointCost(Ability::Charisma) && _attributes.charisma() < kMaxAbilityScore);
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
    _controls.LB_DESC->clearItems();
    _controls.LB_DESC->addTextLinesAsItems(description);
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
