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

#include "reone/game/gui/ingame/abilities.h"

#include "reone/graphics/textures.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/resource/2da.h"
#include "reone/resource/2das.h"
#include "reone/resource/di/services.h"
#include "reone/resource/strings.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/party.h"

using namespace std;

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefSkillRank = 1579;
static constexpr int kStrRefBonus = 32129;
static constexpr int kStrRefTotalRank = 41904;

void AbilitiesMenu::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    _binding.btnSkills->setDisabled(true);
    _binding.btnPowers->setDisabled(true);
    _binding.btnFeats->setDisabled(true);

    _binding.lblSkillRank->setTextMessage(_services.resource.strings.get(kStrRefSkillRank));
    _binding.lblBonus->setTextMessage(_services.resource.strings.get(kStrRefBonus));
    _binding.lblTotal->setTextMessage(_services.resource.strings.get(kStrRefTotalRank));
    _binding.lblRankVal->setTextMessage("");
    _binding.lblBonusVal->setTextMessage("");
    _binding.lblTotalVal->setTextMessage("");
    _binding.lblName->setTextMessage("");

    _binding.lbDesc->setProtoMatchContent(true);
    _binding.lbAbility->setOnItemClick([this](const string &item) {
        auto skill = static_cast<SkillType>(stoi(item));
        auto maybeSkillInfo = _skills.find(skill);
        if (maybeSkillInfo == _skills.end())
            return;

        shared_ptr<Creature> partyLeader(_game.party().getLeader());

        _binding.lblRankVal->setTextMessage(to_string(partyLeader->attributes().getSkillRank(skill)));
        _binding.lblBonusVal->setTextMessage("0");
        _binding.lblTotalVal->setTextMessage(to_string(partyLeader->attributes().getSkillRank(skill)));
        _binding.lblName->setTextMessage(maybeSkillInfo->second.name);

        _binding.lbDesc->clearItems();
        _binding.lbDesc->addTextLinesAsItems(maybeSkillInfo->second.description);
    });
    _binding.btnExit->setOnClick([this]() {
        _game.openInGame();
    });

    loadSkills();
}

void AbilitiesMenu::bindControls() {
    _binding.btnExit = getControl<Button>("BTN_EXIT");
    _binding.btnFeats = getControl<Button>("BTN_FEATS");
    _binding.btnPowers = getControl<Button>("BTN_POWERS");
    _binding.btnSkills = getControl<Label>("BTN_SKILLS");
    _binding.lblBonus = getControl<Label>("LBL_BONUS");
    _binding.lblBonusVal = getControl<Label>("LBL_BONUSVAL");
    _binding.lblInfoBg = getControl<Label>("LBL_INFOBG");
    _binding.lblName = getControl<Label>("LBL_NAME");
    _binding.lblRankVal = getControl<Label>("LBL_RANKVAL");
    _binding.lblSkillRank = getControl<Label>("LBL_SKILLRANK");
    _binding.lblTotal = getControl<Label>("LBL_TOTAL");
    _binding.lblTotalVal = getControl<Label>("LBL_TOTALVAL");
    _binding.lbAbility = getControl<ListBox>("LB_ABILITY");
    _binding.lbDesc = getControl<ListBox>("LB_DESC");

    if (!_game.isTSL()) {
        _binding.btnChange1 = getControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControl<Button>("BTN_CHANGE2");
        _binding.lblPortrait = getControl<Label>("LBL_PORTRAIT");
    } else {
        _binding.lblAbilities = getControl<Label>("LBL_ABILITIES");
        _binding.lblBar1 = getControl<Label>("LBL_BAR1");
        _binding.lblBar2 = getControl<Label>("LBL_BAR2");
        _binding.lblBar3 = getControl<Label>("LBL_BAR3");
        _binding.lblBar4 = getControl<Label>("LBL_BAR4");
        _binding.lblBar5 = getControl<Label>("LBL_BAR5");
        _binding.lblBar6 = getControl<Label>("LBL_BAR6");
        _binding.lblFilter = getControl<Label>("LBL_FILTER");
        _binding.lbDescFeats = getControl<ListBox>("LB_DESC_FEATS");
    }
}

void AbilitiesMenu::loadSkills() {
    shared_ptr<TwoDa> skills(_services.resource.twoDas.get("skills"));
    for (int row = 0; row < skills->getRowCount(); ++row) {
        auto skill = static_cast<SkillType>(row);

        SkillInfo skillInfo;
        skillInfo.skill = skill;
        skillInfo.name = _services.resource.strings.get(skills->getInt(row, "name"));
        skillInfo.description = _services.resource.strings.get(skills->getInt(row, "description"));
        skillInfo.icon = _services.graphics.textures.get(skills->getString(row, "icon"), TextureUsage::GUI);

        _skills.insert(make_pair(skill, std::move(skillInfo)));
    }

    _binding.lbAbility->clearItems();
    for (auto &skill : _skills) {
        ListBox::Item item;
        item.tag = to_string(static_cast<int>(skill.second.skill));
        item.text = skill.second.name;
        item.iconFrame = getFrameTexture();
        item.iconTexture = skill.second.icon;
        _binding.lbAbility->addItem(std::move(item));
    }
}

shared_ptr<Texture> AbilitiesMenu::getFrameTexture() const {
    string resRef;
    if (_game.isTSL()) {
        resRef = "uibit_eqp_itm1";
    } else {
        resRef = "lbl_hex_3";
    }
    return _services.graphics.textures.get(resRef, TextureUsage::GUI);
}

void AbilitiesMenu::refreshControls() {
    refreshPortraits();
}

void AbilitiesMenu::refreshPortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    shared_ptr<Creature> partyLeader(party.getLeader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

} // namespace game

} // namespace reone
