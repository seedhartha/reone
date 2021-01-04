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
    GUI(version, opts),
    _charGen(charGen) {

    _resRef = getResRef("skchrgen");

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
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

void CharGenSkills::reset() {
    _attributes = _charGen->character().attributes();

    shared_ptr<CreatureClass> clazz(Classes::instance().get(_charGen->character().getClass()));
    _points = glm::max(4, (clazz->skillPointBase() + _attributes.getAbilityModifier(Ability::Intelligence)) / 2);

    refreshControls();
}

void CharGenSkills::refreshControls() {
    setControlText("REMAINING_SELECTIONS_LBL", to_string(_points));

    setControlText("COMPUTER_USE_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::ComputerUse)));
    setControlText("DEMOLITIONS_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::Demolitions)));
    setControlText("STEALTH_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::Stealth)));
    setControlText("AWARENESS_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::Awareness)));
    setControlText("PERSUADE_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::Persuade)));
    setControlText("REPAIR_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::Repair)));
    setControlText("SECURITY_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::Security)));
    setControlText("TREAT_INJURY_POINTS_BTN", to_string(_attributes.getSkillRank(Skill::TreatInjury)));

    setControlVisible("COM_MINUS_BTN", _attributes.getSkillRank(Skill::ComputerUse) > 0);
    setControlVisible("DEM_MINUS_BTN", _attributes.getSkillRank(Skill::Demolitions) > 0);
    setControlVisible("STE_MINUS_BTN", _attributes.getSkillRank(Skill::Stealth) > 0);
    setControlVisible("AWA_MINUS_BTN", _attributes.getSkillRank(Skill::Awareness) > 0);
    setControlVisible("PER_MINUS_BTN", _attributes.getSkillRank(Skill::Persuade) > 0);
    setControlVisible("REP_MINUS_BTN", _attributes.getSkillRank(Skill::Repair) > 0);
    setControlVisible("SEC_MINUS_BTN", _attributes.getSkillRank(Skill::Security) > 0);
    setControlVisible("TRE_MINUS_BTN", _attributes.getSkillRank(Skill::TreatInjury) > 0);

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
    ClassType clazz = _charGen->character().getClass();

    shared_ptr<CreatureClass> creatureClass(Classes::instance().get(clazz));
    int maxSkillRank = creatureClass->isClassSkill(skill) ? 4 : 2;
    int pointCost = creatureClass->isClassSkill(skill) ? 1 : 2;

    return _points >= pointCost && _attributes.getSkillRank(Skill::ComputerUse) < maxSkillRank;
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
        _attributes.setSkillRank(skill, _attributes.getSkillRank(skill) - 1);
        _points += getPointCost(skill);
        refreshControls();
    } else if (boost::ends_with(control, "_PLUS_BTN")) {
        Skill skill = getSkillByAlias(control.substr(0, 3));
        _points -= getPointCost(skill);
        _attributes.setSkillRank(skill, _attributes.getSkillRank(skill) + 1);
        refreshControls();
    }
}

void CharGenSkills::updateCharacter() {
    StaticCreatureBlueprint blueprint(_charGen->character());
    blueprint.setAttributes(_attributes);
    _charGen->setCharacter(move(blueprint));
}

int CharGenSkills::getPointCost(Skill skill) const {
    ClassType clazz = _charGen->character().getClass();
    shared_ptr<CreatureClass> creatureClass(Classes::instance().get(clazz));
    return creatureClass->isClassSkill(skill) ? 1 : 2;
}

} // namespace game

} // namespace reone
