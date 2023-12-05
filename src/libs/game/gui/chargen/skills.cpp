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

#include "reone/game/gui/chargen/skills.h"

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/resource/strings.h"

using namespace reone::audio;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static const std::unordered_map<SkillType, int> g_descStrRefBySkill {
    {SkillType::ComputerUse, 244},
    {SkillType::Demolitions, 246},
    {SkillType::Stealth, 248},
    {SkillType::Awareness, 250},
    {SkillType::Persuade, 252},
    {SkillType::Repair, 254},
    {SkillType::Security, 256},
    {SkillType::TreatInjury, 258}};

void CharGenSkills::onGUILoaded() {
    bindControls();

    std::vector<Label *> skillLabels {
        _controls.COMPUTER_USE_LBL.get(),
        _controls.DEMOLITIONS_LBL.get(),
        _controls.STEALTH_LBL.get(),
        _controls.AWARENESS_LBL.get(),
        _controls.PERSUADE_LBL.get(),
        _controls.REPAIR_LBL.get(),
        _controls.SECURITY_LBL.get(),
        _controls.TREAT_INJURY_LBL.get()};
    for (auto &label : skillLabels) {
        label->setSelectable(true);
        label->setHilightColor(_baseColor);
    }

    _controls.LB_DESC->setProtoMatchContent(true);

    _controls.COMPUTER_USE_POINTS_BTN->setDisabled(true);
    _controls.DEMOLITIONS_POINTS_BTN->setDisabled(true);
    _controls.STEALTH_POINTS_BTN->setDisabled(true);
    _controls.AWARENESS_POINTS_BTN->setDisabled(true);
    _controls.PERSUADE_POINTS_BTN->setDisabled(true);
    _controls.REPAIR_POINTS_BTN->setDisabled(true);
    _controls.SECURITY_POINTS_BTN->setDisabled(true);
    _controls.TREAT_INJURY_POINTS_BTN->setDisabled(true);

    _controls.BTN_RECOMMENDED->setDisabled(true);
    _controls.COST_POINTS_LBL->setTextMessage("");

    _controls.BTN_ACCEPT->setOnClick([this]() {
        if (_points > 0) {
            updateCharacter();
        }
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _controls.BTN_BACK->setOnClick([this]() {
        _charGen.openSteps();
    });

    _controls.COMPUTER_USE_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::ComputerUse, selected);
    });
    _controls.DEMOLITIONS_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::Demolitions, selected);
    });
    _controls.STEALTH_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::Stealth, selected);
    });
    _controls.AWARENESS_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::Awareness, selected);
    });
    _controls.PERSUADE_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::Persuade, selected);
    });
    _controls.REPAIR_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::Repair, selected);
    });
    _controls.SECURITY_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::Security, selected);
    });
    _controls.TREAT_INJURY_LBL->setOnSelectionChanged([this](bool selected) {
        onSkillLabelSelectionChanged(SkillType::TreatInjury, selected);
    });

    _controls.COM_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::ComputerUse);
    });
    _controls.DEM_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::Demolitions);
    });
    _controls.STE_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::Stealth);
    });
    _controls.AWA_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::Awareness);
    });
    _controls.PER_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::Persuade);
    });
    _controls.REP_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::Repair);
    });
    _controls.SEC_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::Security);
    });
    _controls.TRE_MINUS_BTN->setOnClick([this]() {
        onMinusButtonClick(SkillType::TreatInjury);
    });

    _controls.COM_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::ComputerUse);
    });
    _controls.DEM_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::Demolitions);
    });
    _controls.STE_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::Stealth);
    });
    _controls.AWA_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::Awareness);
    });
    _controls.PER_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::Persuade);
    });
    _controls.REP_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::Repair);
    });
    _controls.SEC_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::Security);
    });
    _controls.TRE_PLUS_BTN->setOnClick([this]() {
        onPlusButtonClick(SkillType::TreatInjury);
    });
}

void CharGenSkills::reset(bool newGame) {
    const CreatureAttributes &attributes = _charGen.character().attributes;
    std::shared_ptr<CreatureClass> clazz(_services.game.classes.get(attributes.getEffectiveClass()));

    _points = glm::max(1, (clazz->skillPointBase() + attributes.getAbilityModifier(Ability::Intelligence)) / 2);

    if (newGame) {
        _points *= 4;

        _attributes.setSkillRank(SkillType::ComputerUse, 0);
        _attributes.setSkillRank(SkillType::Demolitions, 0);
        _attributes.setSkillRank(SkillType::Stealth, 0);
        _attributes.setSkillRank(SkillType::Awareness, 0);
        _attributes.setSkillRank(SkillType::Persuade, 0);
        _attributes.setSkillRank(SkillType::Repair, 0);
        _attributes.setSkillRank(SkillType::Security, 0);
        _attributes.setSkillRank(SkillType::TreatInjury, 0);
    } else {
        _attributes = attributes;
    }

    refreshControls();
}

void CharGenSkills::refreshControls() {
    _controls.REMAINING_SELECTIONS_LBL->setTextMessage(std::to_string(_points));

    _controls.COMPUTER_USE_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::ComputerUse)));
    _controls.DEMOLITIONS_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Demolitions)));
    _controls.STEALTH_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Stealth)));
    _controls.AWARENESS_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Awareness)));
    _controls.PERSUADE_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Persuade)));
    _controls.REPAIR_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Repair)));
    _controls.SECURITY_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Security)));
    _controls.TREAT_INJURY_POINTS_BTN->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::TreatInjury)));

    _controls.COM_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::ComputerUse) > 0);
    _controls.DEM_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::Demolitions) > 0);
    _controls.STE_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::Stealth) > 0);
    _controls.AWA_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::Awareness) > 0);
    _controls.PER_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::Persuade) > 0);
    _controls.REP_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::Repair) > 0);
    _controls.SEC_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::Security) > 0);
    _controls.TRE_MINUS_BTN->setVisible(_attributes.getSkillRank(SkillType::TreatInjury) > 0);

    _controls.COM_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::ComputerUse));
    _controls.DEM_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::Demolitions));
    _controls.STE_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::Stealth));
    _controls.AWA_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::Awareness));
    _controls.PER_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::Persuade));
    _controls.REP_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::Repair));
    _controls.SEC_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::Security));
    _controls.TRE_PLUS_BTN->setVisible(canIncreaseSkill(SkillType::TreatInjury));
}

bool CharGenSkills::canIncreaseSkill(SkillType skill) const {
    ClassType clazz = _charGen.character().attributes.getEffectiveClass();

    std::shared_ptr<CreatureClass> creatureClass(_services.game.classes.get(clazz));
    int maxSkillRank = creatureClass->isClassSkill(skill) ? 4 : 2;
    int pointCost = creatureClass->isClassSkill(skill) ? 1 : 2;

    return _points >= pointCost && _attributes.getSkillRank(skill) < maxSkillRank;
}

void CharGenSkills::updateCharacter() {
    Character character(_charGen.character());
    for (auto &skillRank : _attributes.skillRanks()) {
        character.attributes.setSkillRank(skillRank.first, skillRank.second);
    }
    _charGen.setCharacter(std::move(character));
}

int CharGenSkills::getPointCost(SkillType skill) const {
    ClassType clazz = _charGen.character().attributes.getEffectiveClass();
    std::shared_ptr<CreatureClass> creatureClass(_services.game.classes.get(clazz));
    return creatureClass->isClassSkill(skill) ? 1 : 2;
}

void CharGenSkills::onMinusButtonClick(SkillType skill) {
    _attributes.setSkillRank(skill, _attributes.getSkillRank(skill) - 1);
    _points += getPointCost(skill);
    refreshControls();
}

void CharGenSkills::onPlusButtonClick(SkillType skill) {
    _points -= getPointCost(skill);
    _attributes.setSkillRank(skill, _attributes.getSkillRank(skill) + 1);
    refreshControls();
}

void CharGenSkills::onSkillLabelSelectionChanged(SkillType skill, bool selected) {
    if (!selected)
        return;

    auto maybeDescription = g_descStrRefBySkill.find(skill);
    if (maybeDescription == g_descStrRefBySkill.end())
        return;

    std::string description(_services.resource.strings.getText(maybeDescription->second));
    _controls.LB_DESC->clearItems();
    _controls.LB_DESC->addTextLinesAsItems(description);
}

} // namespace game

} // namespace reone
