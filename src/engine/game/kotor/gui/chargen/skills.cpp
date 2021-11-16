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

#include "../../../../gui/control/button.h"
#include "../../../../gui/control/label.h"
#include "../../../../gui/control/listbox.h"
#include "../../../../resource/strings.h"

#include "../../../core/d20/classes.h"

#include "../../kotor.h"

#include "chargen.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static const unordered_map<SkillType, int> g_descStrRefBySkill {
    {SkillType::ComputerUse, 244},
    {SkillType::Demolitions, 246},
    {SkillType::Stealth, 248},
    {SkillType::Awareness, 250},
    {SkillType::Persuade, 252},
    {SkillType::Repair, 254},
    {SkillType::Security, 256},
    {SkillType::TreatInjury, 258}};

CharGenSkills::CharGenSkills(
    CharacterGeneration *charGen,
    KotOR *game,
    ActionFactory &actionFactory,
    Classes &classes,
    Combat &combat,
    Feats &feats,
    FootstepSounds &footstepSounds,
    GUISounds &guiSounds,
    ObjectFactory &objectFactory,
    Party &party,
    Portraits &portraits,
    Reputes &reputes,
    ScriptRunner &scriptRunner,
    SoundSets &soundSets,
    Surfaces &surfaces,
    AudioFiles &audioFiles,
    AudioPlayer &audioPlayer,
    Context &context,
    Features &features,
    Fonts &fonts,
    Lips &lips,
    Materials &materials,
    Meshes &meshes,
    Models &models,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    Walkmeshes &walkmeshes,
    Window &window,
    Gffs &gffs,
    Resources &resources,
    Strings &strings,
    TwoDas &twoDas) :
    GameGUI(
        game,
        actionFactory,
        classes,
        combat,
        feats,
        footstepSounds,
        guiSounds,
        objectFactory,
        party,
        portraits,
        reputes,
        scriptRunner,
        soundSets,
        surfaces,
        audioFiles,
        audioPlayer,
        context,
        features,
        fonts,
        lips,
        materials,
        meshes,
        models,
        pbrIbl,
        shaders,
        textures,
        walkmeshes,
        window,
        gffs,
        resources,
        strings,
        twoDas),
    _charGen(charGen) {

    _resRef = getResRef("skchrgen");

    initForGame();
}

void CharGenSkills::load() {
    GUI::load();
    bindControls();

    vector<Label *> skillLabels {
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
        label->setHilightColor(_game->getGUIColorBase());
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
        _charGen->goToNextStep();
        _charGen->openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen->openSteps();
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
    _binding.btnAccept = getControl<Button>("BTN_ACCEPT");
    _binding.btnBack = getControl<Button>("BTN_BACK");

    _binding.awaMinusBtn = getControl<Button>("AWA_MINUS_BTN");
    _binding.awaPlusBtn = getControl<Button>("AWA_PLUS_BTN");
    _binding.awarenessPointsBtn = getControl<Button>("AWARENESS_POINTS_BTN");
    _binding.awarenessLbl = getControl<Label>("AWARENESS_LBL");
    _binding.btnRecommended = getControl<Button>("BTN_RECOMMENDED");
    _binding.comMinusBtn = getControl<Button>("COM_MINUS_BTN");
    _binding.comPlusBtn = getControl<Button>("COM_PLUS_BTN");
    _binding.computerUsePointsBtn = getControl<Button>("COMPUTER_USE_POINTS_BTN");
    _binding.computerUseLbl = getControl<Label>("COMPUTER_USE_LBL");
    _binding.demMinusBtn = getControl<Button>("DEM_MINUS_BTN");
    _binding.demolitionsPointsBtn = getControl<Button>("DEMOLITIONS_POINTS_BTN");
    _binding.demolitionsLbl = getControl<Label>("DEMOLITIONS_LBL");
    _binding.demPlusBtn = getControl<Button>("DEM_PLUS_BTN");
    _binding.perMinusBtn = getControl<Button>("PER_MINUS_BTN");
    _binding.perPlusBtn = getControl<Button>("PER_PLUS_BTN");
    _binding.persuadePointsBtn = getControl<Button>("PERSUADE_POINTS_BTN");
    _binding.persuadeLbl = getControl<Label>("PERSUADE_LBL");
    _binding.repairPointsBtn = getControl<Button>("REPAIR_POINTS_BTN");
    _binding.repairLbl = getControl<Label>("REPAIR_LBL");
    _binding.repMinusBtn = getControl<Button>("REP_MINUS_BTN");
    _binding.repPlusBtn = getControl<Button>("REP_PLUS_BTN");
    _binding.secMinusBtn = getControl<Button>("SEC_MINUS_BTN");
    _binding.secPlusBtn = getControl<Button>("SEC_PLUS_BTN");
    _binding.securityPointsBtn = getControl<Button>("SECURITY_POINTS_BTN");
    _binding.securityLbl = getControl<Label>("SECURITY_LBL");
    _binding.stealthPointsBtn = getControl<Button>("STEALTH_POINTS_BTN");
    _binding.stealthLbl = getControl<Label>("STEALTH_LBL");
    _binding.steMinusBtn = getControl<Button>("STE_MINUS_BTN");
    _binding.stePlusBtn = getControl<Button>("STE_PLUS_BTN");
    _binding.treatInjuryPointsBtn = getControl<Button>("TREAT_INJURY_POINTS_BTN");
    _binding.treatInjuryLbl = getControl<Label>("TREAT_INJURY_LBL");
    _binding.treMinusBtn = getControl<Button>("TRE_MINUS_BTN");
    _binding.trePlusBtn = getControl<Button>("TRE_PLUS_BTN");
    _binding.costPointsLbl = getControl<Label>("COST_POINTS_LBL");
    _binding.remainingSelectionsLbl = getControl<Label>("REMAINING_SELECTIONS_LBL");
    _binding.lbDesc = getControl<ListBox>("LB_DESC");
}

void CharGenSkills::reset(bool newGame) {
    const CreatureAttributes &attributes = _charGen->character().attributes;
    shared_ptr<CreatureClass> clazz(_classes.get(attributes.getEffectiveClass()));

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

    shared_ptr<CreatureClass> creatureClass(_classes.get(clazz));
    int maxSkillRank = creatureClass->isClassSkill(skill) ? 4 : 2;
    int pointCost = creatureClass->isClassSkill(skill) ? 1 : 2;

    return _points >= pointCost && _attributes.getSkillRank(skill) < maxSkillRank;
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
    shared_ptr<CreatureClass> creatureClass(_classes.get(clazz));
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

    string description(_strings.get(maybeDescription->second));
    _binding.lbDesc->clearItems();
    _binding.lbDesc->addTextLinesAsItems(description);
}

} // namespace game

} // namespace reone
