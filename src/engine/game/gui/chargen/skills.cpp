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

#include "skills.h"

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

static const unordered_map<string, SkillType> g_skillByAlias {
    { "COM", SkillType::ComputerUse },
    { "DEM", SkillType::Demolitions },
    { "STE", SkillType::Stealth },
    { "AWA", SkillType::Awareness },
    { "PER", SkillType::Persuade },
    { "REP", SkillType::Repair },
    { "SEC", SkillType::Security },
    { "TRE", SkillType::TreatInjury }
};

static const unordered_map<string, SkillType> g_skillByLabelTag {
    { "COMPUTER_USE_LBL", SkillType::ComputerUse },
    { "DEMOLITIONS_LBL", SkillType::Demolitions },
    { "STEALTH_LBL", SkillType::Stealth },
    { "AWARENESS_LBL", SkillType::Awareness },
    { "PERSUADE_LBL", SkillType::Persuade },
    { "REPAIR_LBL", SkillType::Repair },
    { "SECURITY_LBL", SkillType::Security },
    { "TREAT_INJURY_LBL", SkillType::TreatInjury }
};

static const unordered_map<SkillType, int> g_descStrRefBySkill {
    { SkillType::ComputerUse, 244 },
    { SkillType::Demolitions, 246 },
    { SkillType::Stealth, 248  },
    { SkillType::Awareness, 250 },
    { SkillType::Persuade, 252 },
    { SkillType::Repair, 254 },
    { SkillType::Security, 256  },
    { SkillType::TreatInjury, 258 }
};

CharGenSkills::CharGenSkills(CharacterGeneration *charGen, Game *game) :
    GameGUI(game),
    _charGen(charGen) {

    _resRef = getResRef("skchrgen");

    initForGame();
}

void CharGenSkills::load() {
    GUI::load();
    bindControls();

    for (auto &skill : g_skillByLabelTag) {
        configureControl(skill.first, [this](Control &control) {
            control.setFocusable(true);
            control.setHilightColor(_game->getGUIColorBase());
        });
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
}

void CharGenSkills::bindControls() {
    _binding.awaMinusBtn = getControlPtr<Button>("AWA_MINUS_BTN");
    _binding.awaPlusBtn = getControlPtr<Button>("AWA_PLUS_BTN");
    _binding.awarenessPointsBtn = getControlPtr<Button>("AWARENESS_POINTS_BTN");
    _binding.btnRecommended = getControlPtr<Button>("BTN_RECOMMENDED");
    _binding.comMinusBtn = getControlPtr<Button>("COM_MINUS_BTN");
    _binding.comPlusBtn = getControlPtr<Button>("COM_PLUS_BTN");
    _binding.computerUsePointsBtn = getControlPtr<Button>("COMPUTER_USE_POINTS_BTN");
    _binding.demMinusBtn = getControlPtr<Button>("DEM_MINUS_BTN");
    _binding.demolitionsPointsBtn = getControlPtr<Button>("DEMOLITIONS_POINTS_BTN");
    _binding.demPlusBtn = getControlPtr<Button>("DEM_PLUS_BTN");
    _binding.perMinusBtn = getControlPtr<Button>("PER_MINUS_BTN");
    _binding.perPlusBtn = getControlPtr<Button>("PER_PLUS_BTN");
    _binding.persuadePointsBtn = getControlPtr<Button>("PERSUADE_POINTS_BTN");
    _binding.repairPointsBtn = getControlPtr<Button>("REPAIR_POINTS_BTN");
    _binding.repMinusBtn = getControlPtr<Button>("REP_MINUS_BTN");
    _binding.repPlusBtn = getControlPtr<Button>("REP_PLUS_BTN");
    _binding.secMinusBtn = getControlPtr<Button>("SEC_MINUS_BTN");
    _binding.secPlusBtn = getControlPtr<Button>("SEC_PLUS_BTN");
    _binding.securityPointsBtn = getControlPtr<Button>("SECURITY_POINTS_BTN");
    _binding.stealthPointsBtn = getControlPtr<Button>("STEALTH_POINTS_BTN");
    _binding.steMinusBtn = getControlPtr<Button>("STE_MINUS_BTN");
    _binding.stePlusBtn = getControlPtr<Button>("STE_PLUS_BTN");
    _binding.treatInjuryPointsBtn = getControlPtr<Button>("TREAT_INJURY_POINTS_BTN");
    _binding.treMinusBtn = getControlPtr<Button>("TRE_MINUS_BTN");
    _binding.trePlusBtn = getControlPtr<Button>("TRE_PLUS_BTN");
    _binding.costPointsLbl = getControlPtr<Label>("COST_POINTS_LBL");
    _binding.remainingSelectionsLbl = getControlPtr<Label>("REMAINING_SELECTIONS_LBL");
    _binding.lbDesc = getControlPtr<ListBox>("LB_DESC");
}

void CharGenSkills::reset(bool newGame) {
    const CreatureAttributes &attributes = _charGen->character().attributes;
    shared_ptr<CreatureClass> clazz(_game->services().classes().get(attributes.getEffectiveClass()));

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
    _binding.remainingSelectionsLbl->setTextMessage(to_string(_points));

    _binding.computerUsePointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::ComputerUse)));
    _binding.demolitionsPointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::Demolitions)));
    _binding.stealthPointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::Stealth)));
    _binding.awarenessPointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::Awareness)));
    _binding.persuadePointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::Persuade)));
    _binding.repairPointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::Repair)));
    _binding.securityPointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::Security)));
    _binding.treatInjuryPointsBtn->setTextMessage(to_string(_attributes.getSkillRank(SkillType::TreatInjury)));

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
    ClassType clazz = _charGen->character().attributes.getEffectiveClass();

    shared_ptr<CreatureClass> creatureClass(_game->services().classes().get(clazz));
    int maxSkillRank = creatureClass->isClassSkill(skill) ? 4 : 2;
    int pointCost = creatureClass->isClassSkill(skill) ? 1 : 2;

    return _points >= pointCost && _attributes.getSkillRank(skill) < maxSkillRank;
}

static SkillType getSkillByAlias(const string &alias) {
    return g_skillByAlias.find(alias)->second;
}

void CharGenSkills::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_ACCEPT") {
        if (_points == 0) {
            updateCharacter();
            _charGen->goToNextStep();
            _charGen->openSteps();
        }
        _charGen->goToNextStep();
        _charGen->openSteps();

    } else if (control == "BTN_BACK") {
        _charGen->openSteps();

    } else if (boost::ends_with(control, "_MINUS_BTN")) {
        SkillType skill = getSkillByAlias(control.substr(0, 3));
        _attributes.setSkillRank(skill, _attributes.getSkillRank(skill) - 1);
        _points += getPointCost(skill);
        refreshControls();

    } else if (boost::ends_with(control, "_PLUS_BTN")) {
        SkillType skill = getSkillByAlias(control.substr(0, 3));
        _points -= getPointCost(skill);
        _attributes.setSkillRank(skill, _attributes.getSkillRank(skill) + 1);
        refreshControls();
    }
}

void CharGenSkills::updateCharacter() {
    Character character(_charGen->character());
    for (auto &skillRank : _attributes.skillRanks()) {
        character.attributes.setSkillRank(skillRank.first, skillRank.second);
    }
    _charGen->setCharacter(move(character));
}

int CharGenSkills::getPointCost(SkillType skill) const {
    ClassType clazz = _charGen->character().attributes.getEffectiveClass();
    shared_ptr<CreatureClass> creatureClass(_game->services().classes().get(clazz));
    return creatureClass->isClassSkill(skill) ? 1 : 2;
}

void CharGenSkills::onFocusChanged(const string &control, bool focus) {
    auto maybeSkill = g_skillByLabelTag.find(control);
    if (maybeSkill != g_skillByLabelTag.end() && focus) {
        auto maybeDescription = g_descStrRefBySkill.find(maybeSkill->second);
        if (maybeDescription != g_descStrRefBySkill.end()) {
            string description(_game->services().resource().strings().get(maybeDescription->second));
            _binding.lbDesc->clearItems();
            _binding.lbDesc->addTextLinesAsItems(description);
        }
    }
}

} // namespace game

} // namespace reone
