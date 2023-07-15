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

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/resource/strings.h"

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"

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
        _binding.computerUseLbl.get(),
        _binding.demolitionsLbl.get(),
        _binding.stealthLbl.get(),
        _binding.awarenessLbl.get(),
        _binding.persuadeLbl.get(),
        _binding.repairLbl.get(),
        _binding.securityLbl.get(),
        _binding.treatInjuryLbl.get()};
    for (auto &label : skillLabels) {
        label->setFocusable(true);
        label->setHilightColor(_baseColor);
    }

    _binding.lbDesc->setProtoMatchContent(true);

    _binding.computerUsePointsBtn->setDisabled(true);
    _binding.demolitionsPointsBtn->setDisabled(true);
    _binding.stealthPointsBtn->setDisabled(true);
    _binding.awarenessPointsBtn->setDisabled(true);
    _binding.persuadePointsBtn->setDisabled(true);
    _binding.repairPointsBtn->setDisabled(true);
    _binding.securityPointsBtn->setDisabled(true);
    _binding.treatInjuryPointsBtn->setDisabled(true);

    _binding.btnRecommended->setDisabled(true);
    _binding.costPointsLbl->setTextMessage("");

    _binding.btnAccept->setOnClick([this]() {
        if (_points > 0) {
            updateCharacter();
        }
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen.openSteps();
    });

    _binding.computerUseLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::ComputerUse, focus);
    });
    _binding.demolitionsLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::Demolitions, focus);
    });
    _binding.stealthLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::Stealth, focus);
    });
    _binding.awarenessLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::Awareness, focus);
    });
    _binding.persuadeLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::Persuade, focus);
    });
    _binding.repairLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::Repair, focus);
    });
    _binding.securityLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::Security, focus);
    });
    _binding.treatInjuryLbl->setOnFocusChanged([this](bool focus) {
        onSkillLabelFocusChanged(SkillType::TreatInjury, focus);
    });

    _binding.comMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::ComputerUse);
    });
    _binding.demMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::Demolitions);
    });
    _binding.steMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::Stealth);
    });
    _binding.awaMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::Awareness);
    });
    _binding.perMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::Persuade);
    });
    _binding.repMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::Repair);
    });
    _binding.secMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::Security);
    });
    _binding.treMinusBtn->setOnClick([this]() {
        onMinusButtonClick(SkillType::TreatInjury);
    });

    _binding.comPlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::ComputerUse);
    });
    _binding.demPlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::Demolitions);
    });
    _binding.stePlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::Stealth);
    });
    _binding.awaPlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::Awareness);
    });
    _binding.perPlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::Persuade);
    });
    _binding.repPlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::Repair);
    });
    _binding.secPlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::Security);
    });
    _binding.trePlusBtn->setOnClick([this]() {
        onPlusButtonClick(SkillType::TreatInjury);
    });
}

void CharGenSkills::bindControls() {
    _binding.btnAccept = findControl<Button>("BTN_ACCEPT");
    _binding.btnBack = findControl<Button>("BTN_BACK");

    _binding.awaMinusBtn = findControl<Button>("AWA_MINUS_BTN");
    _binding.awaPlusBtn = findControl<Button>("AWA_PLUS_BTN");
    _binding.awarenessPointsBtn = findControl<Button>("AWARENESS_POINTS_BTN");
    _binding.awarenessLbl = findControl<Label>("AWARENESS_LBL");
    _binding.btnRecommended = findControl<Button>("BTN_RECOMMENDED");
    _binding.comMinusBtn = findControl<Button>("COM_MINUS_BTN");
    _binding.comPlusBtn = findControl<Button>("COM_PLUS_BTN");
    _binding.computerUsePointsBtn = findControl<Button>("COMPUTER_USE_POINTS_BTN");
    _binding.computerUseLbl = findControl<Label>("COMPUTER_USE_LBL");
    _binding.demMinusBtn = findControl<Button>("DEM_MINUS_BTN");
    _binding.demolitionsPointsBtn = findControl<Button>("DEMOLITIONS_POINTS_BTN");
    _binding.demolitionsLbl = findControl<Label>("DEMOLITIONS_LBL");
    _binding.demPlusBtn = findControl<Button>("DEM_PLUS_BTN");
    _binding.perMinusBtn = findControl<Button>("PER_MINUS_BTN");
    _binding.perPlusBtn = findControl<Button>("PER_PLUS_BTN");
    _binding.persuadePointsBtn = findControl<Button>("PERSUADE_POINTS_BTN");
    _binding.persuadeLbl = findControl<Label>("PERSUADE_LBL");
    _binding.repairPointsBtn = findControl<Button>("REPAIR_POINTS_BTN");
    _binding.repairLbl = findControl<Label>("REPAIR_LBL");
    _binding.repMinusBtn = findControl<Button>("REP_MINUS_BTN");
    _binding.repPlusBtn = findControl<Button>("REP_PLUS_BTN");
    _binding.secMinusBtn = findControl<Button>("SEC_MINUS_BTN");
    _binding.secPlusBtn = findControl<Button>("SEC_PLUS_BTN");
    _binding.securityPointsBtn = findControl<Button>("SECURITY_POINTS_BTN");
    _binding.securityLbl = findControl<Label>("SECURITY_LBL");
    _binding.stealthPointsBtn = findControl<Button>("STEALTH_POINTS_BTN");
    _binding.stealthLbl = findControl<Label>("STEALTH_LBL");
    _binding.steMinusBtn = findControl<Button>("STE_MINUS_BTN");
    _binding.stePlusBtn = findControl<Button>("STE_PLUS_BTN");
    _binding.treatInjuryPointsBtn = findControl<Button>("TREAT_INJURY_POINTS_BTN");
    _binding.treatInjuryLbl = findControl<Label>("TREAT_INJURY_LBL");
    _binding.treMinusBtn = findControl<Button>("TRE_MINUS_BTN");
    _binding.trePlusBtn = findControl<Button>("TRE_PLUS_BTN");
    _binding.costPointsLbl = findControl<Label>("COST_POINTS_LBL");
    _binding.remainingSelectionsLbl = findControl<Label>("REMAINING_SELECTIONS_LBL");
    _binding.lbDesc = findControl<ListBox>("LB_DESC");
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
    _binding.remainingSelectionsLbl->setTextMessage(std::to_string(_points));

    _binding.computerUsePointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::ComputerUse)));
    _binding.demolitionsPointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Demolitions)));
    _binding.stealthPointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Stealth)));
    _binding.awarenessPointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Awareness)));
    _binding.persuadePointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Persuade)));
    _binding.repairPointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Repair)));
    _binding.securityPointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::Security)));
    _binding.treatInjuryPointsBtn->setTextMessage(std::to_string(_attributes.getSkillRank(SkillType::TreatInjury)));

    _binding.comMinusBtn->setVisible(_attributes.getSkillRank(SkillType::ComputerUse) > 0);
    _binding.demMinusBtn->setVisible(_attributes.getSkillRank(SkillType::Demolitions) > 0);
    _binding.steMinusBtn->setVisible(_attributes.getSkillRank(SkillType::Stealth) > 0);
    _binding.awaMinusBtn->setVisible(_attributes.getSkillRank(SkillType::Awareness) > 0);
    _binding.perMinusBtn->setVisible(_attributes.getSkillRank(SkillType::Persuade) > 0);
    _binding.repMinusBtn->setVisible(_attributes.getSkillRank(SkillType::Repair) > 0);
    _binding.secMinusBtn->setVisible(_attributes.getSkillRank(SkillType::Security) > 0);
    _binding.treMinusBtn->setVisible(_attributes.getSkillRank(SkillType::TreatInjury) > 0);

    _binding.comPlusBtn->setVisible(canIncreaseSkill(SkillType::ComputerUse));
    _binding.demPlusBtn->setVisible(canIncreaseSkill(SkillType::Demolitions));
    _binding.stePlusBtn->setVisible(canIncreaseSkill(SkillType::Stealth));
    _binding.awaPlusBtn->setVisible(canIncreaseSkill(SkillType::Awareness));
    _binding.perPlusBtn->setVisible(canIncreaseSkill(SkillType::Persuade));
    _binding.repPlusBtn->setVisible(canIncreaseSkill(SkillType::Repair));
    _binding.secPlusBtn->setVisible(canIncreaseSkill(SkillType::Security));
    _binding.trePlusBtn->setVisible(canIncreaseSkill(SkillType::TreatInjury));
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

void CharGenSkills::onSkillLabelFocusChanged(SkillType skill, bool focus) {
    if (!focus)
        return;

    auto maybeDescription = g_descStrRefBySkill.find(skill);
    if (maybeDescription == g_descStrRefBySkill.end())
        return;

    std::string description(_services.resource.strings.getText(maybeDescription->second));
    _binding.lbDesc->clearItems();
    _binding.lbDesc->addTextLinesAsItems(description);
}

} // namespace game

} // namespace reone
