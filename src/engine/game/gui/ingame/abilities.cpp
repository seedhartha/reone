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

#include "abilities.h"

#include "../../../gui/control/listbox.h"
#include "../../../graphics/texture/textures.h"
#include "../../../resource/resources.h"
#include "../../../resource/strings.h"

#include "../../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefSkillRank = 1579;
static constexpr int kStrRefBonus = 32129;
static constexpr int kStrRefTotalRank = 41904;

AbilitiesMenu::AbilitiesMenu(Game *game) : GameGUI(game) {
    _resRef = getResRef("abilities");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void AbilitiesMenu::load() {
    GUI::load();
    bindControls();

    _binding.btnSkills->setDisabled(true);
    _binding.btnPowers->setDisabled(true);
    _binding.btnFeats->setDisabled(true);

    _binding.lblSkillRank->setTextMessage(_game->services().resource().strings().get(kStrRefSkillRank));
    _binding.lblBonus->setTextMessage(_game->services().resource().strings().get(kStrRefBonus));
    _binding.lblTotal->setTextMessage(_game->services().resource().strings().get(kStrRefTotalRank));
    _binding.lblRankVal->setTextMessage("");
    _binding.lblBonusVal->setTextMessage("");
    _binding.lblTotalVal->setTextMessage("");
    _binding.lblName->setTextMessage("");

    _binding.lbDesc->setProtoMatchContent(true);

    loadSkills();
}

void AbilitiesMenu::bindControls() {
    _binding.btnExit = getControlPtr<Button>("BTN_EXIT");
    _binding.btnFeats = getControlPtr<Button>("BTN_FEATS");
    _binding.btnPowers = getControlPtr<Button>("BTN_POWERS");
    _binding.btnSkills = getControlPtr<Label>("BTN_SKILLS");
    _binding.lblBonus = getControlPtr<Label>("LBL_BONUS");
    _binding.lblBonusVal = getControlPtr<Label>("LBL_BONUSVAL");
    _binding.lblInfoBg = getControlPtr<Label>("LBL_INFOBG");
    _binding.lblName = getControlPtr<Label>("LBL_NAME");
    _binding.lblRankVal = getControlPtr<Label>("LBL_RANKVAL");
    _binding.lblSkillRank = getControlPtr<Label>("LBL_SKILLRANK");
    _binding.lblTotal = getControlPtr<Label>("LBL_TOTAL");
    _binding.lblTotalVal = getControlPtr<Label>("LBL_TOTALVAL");
    _binding.lbAbility = getControlPtr<ListBox>("LB_ABILITY");
    _binding.lbDesc = getControlPtr<ListBox>("LB_DESC");

    if (_game->isKotOR()) {
        _binding.btnChange1 = getControlPtr<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControlPtr<Button>("BTN_CHANGE2");
        _binding.lblPortrait = getControlPtr<Label>("LBL_PORTRAIT");
    } else {
        _binding.lblAbilities = getControlPtr<Label>("LBL_ABILITIES");
        _binding.lblBar1 = getControlPtr<Label>("LBL_BAR1");
        _binding.lblBar2 = getControlPtr<Label>("LBL_BAR2");
        _binding.lblBar3 = getControlPtr<Label>("LBL_BAR3");
        _binding.lblBar4 = getControlPtr<Label>("LBL_BAR4");
        _binding.lblBar5 = getControlPtr<Label>("LBL_BAR5");
        _binding.lblBar6 = getControlPtr<Label>("LBL_BAR6");
        _binding.lblFilter = getControlPtr<Label>("LBL_FILTER");
        _binding.lbDescFeats = getControlPtr<ListBox>("LB_DESC_FEATS");
    }
}

void AbilitiesMenu::loadSkills() {
    shared_ptr<TwoDA> skills(_game->services().resource().resources().get2DA("skills"));
    for (int row = 0; row < skills->getRowCount(); ++row) {
        auto skill = static_cast<SkillType>(row);

        SkillInfo skillInfo;
        skillInfo.skill = skill;
        skillInfo.name = _game->services().resource().strings().get(skills->getInt(row, "name"));
        skillInfo.description = _game->services().resource().strings().get(skills->getInt(row, "description"));
        skillInfo.icon = _game->services().graphics().textures().get(skills->getString(row, "icon"), TextureUsage::GUI);

        _skills.insert(make_pair(skill, move(skillInfo)));
    }

    _binding.lbAbility->clearItems();
    for (auto &skill : _skills) {
        ListBox::Item item;
        item.tag = to_string(static_cast<int>(skill.second.skill));
        item.text = skill.second.name;
        item.iconFrame = getFrameTexture();
        item.iconTexture = skill.second.icon;
        _binding.lbAbility->addItem(move(item));
    }
}

shared_ptr<Texture> AbilitiesMenu::getFrameTexture() const {
    string resRef;
    if (_game->isTSL()) {
        resRef = "uibit_eqp_itm1";
    } else {
        resRef = "lbl_hex_3";
    }
    return _game->services().graphics().textures().get(resRef, TextureUsage::GUI);
}

void AbilitiesMenu::refreshControls() {
    refreshPortraits();
}

void AbilitiesMenu::refreshPortraits() {
    if (_game->id() != GameID::KotOR) return;

    Party &party = _game->services().party();
    shared_ptr<Creature> partyLeader(party.getLeader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void AbilitiesMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    }
}

void AbilitiesMenu::onListBoxItemClick(const string &control, const string &item) {
    if (control == "LB_ABILITY") {
        auto skill = static_cast<SkillType>(stoi(item));
        auto maybeSkillInfo = _skills.find(skill);
        if (maybeSkillInfo != _skills.end()) {
            shared_ptr<Creature> partyLeader(_game->services().party().getLeader());

            _binding.lblRankVal->setTextMessage(to_string(partyLeader->attributes().getSkillRank(skill)));
            _binding.lblBonusVal->setTextMessage("0");
            _binding.lblTotalVal->setTextMessage(to_string(partyLeader->attributes().getSkillRank(skill)));
            _binding.lblName->setTextMessage(maybeSkillInfo->second.name);

            _binding.lbDesc->clearItems();
            _binding.lbDesc->addTextLinesAsItems(maybeSkillInfo->second.description);
        }
    }
}

} // namespace game

} // namespace reone
