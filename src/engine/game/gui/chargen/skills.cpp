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

#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include "../../../gui/control/listbox.h"
#include "../../../resource/strings.h"

#include "../../d20/classes.h"
#include "../../game.h"

#include "../colorutil.h"

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

    for (auto &skill : g_skillByLabelTag) {
        configureControl(skill.first, [this](Control &control) {
            control.setFocusable(true);
            control.setHilightColor(getBaseColor(_game->gameId()));
        });
    }

    ListBox &lbDesc = getControl<ListBox>("LB_DESC");
    lbDesc.setProtoMatchContent(true);

    disableControl("COMPUTER_USE_POINTS_BTN");
    disableControl("DEMOLITIONS_POINTS_BTN");
    disableControl("STEALTH_POINTS_BTN");
    disableControl("AWARENESS_POINTS_BTN");
    disableControl("PERSUADE_POINTS_BTN");
    disableControl("REPAIR_POINTS_BTN");
    disableControl("SECURITY_POINTS_BTN");
    disableControl("TREAT_INJURY_POINTS_BTN");

    disableControl("BTN_RECOMMENDED");

    setControlText("COST_POINTS_LBL", "");
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
    setControlText("REMAINING_SELECTIONS_LBL", to_string(_points));

    setControlText("COMPUTER_USE_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::ComputerUse)));
    setControlText("DEMOLITIONS_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::Demolitions)));
    setControlText("STEALTH_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::Stealth)));
    setControlText("AWARENESS_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::Awareness)));
    setControlText("PERSUADE_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::Persuade)));
    setControlText("REPAIR_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::Repair)));
    setControlText("SECURITY_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::Security)));
    setControlText("TREAT_INJURY_POINTS_BTN", to_string(_attributes.getSkillRank(SkillType::TreatInjury)));

    setControlVisible("COM_MINUS_BTN", _attributes.getSkillRank(SkillType::ComputerUse) > 0);
    setControlVisible("DEM_MINUS_BTN", _attributes.getSkillRank(SkillType::Demolitions) > 0);
    setControlVisible("STE_MINUS_BTN", _attributes.getSkillRank(SkillType::Stealth) > 0);
    setControlVisible("AWA_MINUS_BTN", _attributes.getSkillRank(SkillType::Awareness) > 0);
    setControlVisible("PER_MINUS_BTN", _attributes.getSkillRank(SkillType::Persuade) > 0);
    setControlVisible("REP_MINUS_BTN", _attributes.getSkillRank(SkillType::Repair) > 0);
    setControlVisible("SEC_MINUS_BTN", _attributes.getSkillRank(SkillType::Security) > 0);
    setControlVisible("TRE_MINUS_BTN", _attributes.getSkillRank(SkillType::TreatInjury) > 0);

    setControlVisible("COM_PLUS_BTN", canIncreaseSkill(SkillType::ComputerUse));
    setControlVisible("DEM_PLUS_BTN", canIncreaseSkill(SkillType::Demolitions));
    setControlVisible("STE_PLUS_BTN", canIncreaseSkill(SkillType::Stealth));
    setControlVisible("AWA_PLUS_BTN", canIncreaseSkill(SkillType::Awareness));
    setControlVisible("PER_PLUS_BTN", canIncreaseSkill(SkillType::Persuade));
    setControlVisible("REP_PLUS_BTN", canIncreaseSkill(SkillType::Repair));
    setControlVisible("SEC_PLUS_BTN", canIncreaseSkill(SkillType::Security));
    setControlVisible("TRE_PLUS_BTN", canIncreaseSkill(SkillType::TreatInjury));
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
            ListBox &listBox = getControl<ListBox>("LB_DESC");
            listBox.clearItems();
            listBox.addTextLinesAsItems(description);
        }
    }
}

} // namespace game

} // namespace reone
