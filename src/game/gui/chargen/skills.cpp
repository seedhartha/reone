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

#include "../../rp/classes.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const unordered_map<string, Skill> g_skillByAlias {
    { "COM", Skill::ComputerUse },
    { "DEM", Skill::Demolitions },
    { "STE", Skill::Stealth },
    { "AWA", Skill::Awareness },
    { "PER", Skill::Persuade },
    { "REP", Skill::Repair },
    { "SEC", Skill::Security },
    { "TRE", Skill::TreatInjury }
};

CharGenSkills::CharGenSkills(CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GameGUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("skchrgen");

    initForGame();
}

void CharGenSkills::load() {
    GUI::load();

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
    CreatureAttributes &attributes = _charGen->character().attributes();
    shared_ptr<CreatureClass> clazz(Classes::instance().get(attributes.getEffectiveClass()));

    _points = glm::max(1, (clazz->skillPointBase() + attributes.abilities().getModifier(Ability::Intelligence)) / 2);

    if (newGame) {
        _points *= 4;

        _skills.setRank(Skill::ComputerUse, 0);
        _skills.setRank(Skill::Demolitions, 0);
        _skills.setRank(Skill::Stealth, 0);
        _skills.setRank(Skill::Awareness, 0);
        _skills.setRank(Skill::Persuade, 0);
        _skills.setRank(Skill::Repair, 0);
        _skills.setRank(Skill::Security, 0);
        _skills.setRank(Skill::TreatInjury, 0);
    } else {
        _skills = attributes.skills();
    }

    refreshControls();
}

void CharGenSkills::refreshControls() {
    setControlText("REMAINING_SELECTIONS_LBL", to_string(_points));

    setControlText("COMPUTER_USE_POINTS_BTN", to_string(_skills.getRank(Skill::ComputerUse)));
    setControlText("DEMOLITIONS_POINTS_BTN", to_string(_skills.getRank(Skill::Demolitions)));
    setControlText("STEALTH_POINTS_BTN", to_string(_skills.getRank(Skill::Stealth)));
    setControlText("AWARENESS_POINTS_BTN", to_string(_skills.getRank(Skill::Awareness)));
    setControlText("PERSUADE_POINTS_BTN", to_string(_skills.getRank(Skill::Persuade)));
    setControlText("REPAIR_POINTS_BTN", to_string(_skills.getRank(Skill::Repair)));
    setControlText("SECURITY_POINTS_BTN", to_string(_skills.getRank(Skill::Security)));
    setControlText("TREAT_INJURY_POINTS_BTN", to_string(_skills.getRank(Skill::TreatInjury)));

    setControlVisible("COM_MINUS_BTN", _skills.getRank(Skill::ComputerUse) > 0);
    setControlVisible("DEM_MINUS_BTN", _skills.getRank(Skill::Demolitions) > 0);
    setControlVisible("STE_MINUS_BTN", _skills.getRank(Skill::Stealth) > 0);
    setControlVisible("AWA_MINUS_BTN", _skills.getRank(Skill::Awareness) > 0);
    setControlVisible("PER_MINUS_BTN", _skills.getRank(Skill::Persuade) > 0);
    setControlVisible("REP_MINUS_BTN", _skills.getRank(Skill::Repair) > 0);
    setControlVisible("SEC_MINUS_BTN", _skills.getRank(Skill::Security) > 0);
    setControlVisible("TRE_MINUS_BTN", _skills.getRank(Skill::TreatInjury) > 0);

    setControlVisible("COM_PLUS_BTN", canIncreaseSkill(Skill::ComputerUse));
    setControlVisible("DEM_PLUS_BTN", canIncreaseSkill(Skill::Demolitions));
    setControlVisible("STE_PLUS_BTN", canIncreaseSkill(Skill::Stealth));
    setControlVisible("AWA_PLUS_BTN", canIncreaseSkill(Skill::Awareness));
    setControlVisible("PER_PLUS_BTN", canIncreaseSkill(Skill::Persuade));
    setControlVisible("REP_PLUS_BTN", canIncreaseSkill(Skill::Repair));
    setControlVisible("SEC_PLUS_BTN", canIncreaseSkill(Skill::Security));
    setControlVisible("TRE_PLUS_BTN", canIncreaseSkill(Skill::TreatInjury));
}

bool CharGenSkills::canIncreaseSkill(Skill skill) const {
    ClassType clazz = _charGen->character().attributes().getEffectiveClass();

    shared_ptr<CreatureClass> creatureClass(Classes::instance().get(clazz));
    int maxSkillRank = creatureClass->isClassSkill(skill) ? 4 : 2;
    int pointCost = creatureClass->isClassSkill(skill) ? 1 : 2;

    return _points >= pointCost && _skills.getRank(skill) < maxSkillRank;
}

static Skill getSkillByAlias(const string &alias) {
    return g_skillByAlias.find(alias)->second;
}

void CharGenSkills::onClick(const string &control) {
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
        Skill skill = getSkillByAlias(control.substr(0, 3));
        _skills.setRank(skill, _skills.getRank(skill) - 1);
        _points += getPointCost(skill);
        refreshControls();
    } else if (boost::ends_with(control, "_PLUS_BTN")) {
        Skill skill = getSkillByAlias(control.substr(0, 3));
        _points -= getPointCost(skill);
        _skills.setRank(skill, _skills.getRank(skill) + 1);
        refreshControls();
    }
}

void CharGenSkills::updateCharacter() {
    _charGen->setSkills(_skills);
}

int CharGenSkills::getPointCost(Skill skill) const {
    ClassType clazz = _charGen->character().attributes().getEffectiveClass();
    shared_ptr<CreatureClass> creatureClass(Classes::instance().get(clazz));
    return creatureClass->isClassSkill(skill) ? 1 : 2;
}

} // namespace game

} // namespace reone
