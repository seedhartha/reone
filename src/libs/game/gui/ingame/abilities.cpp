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

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/party.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/resource/2da.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/strings.h"

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

    _controls.BTN_SKILLS->setDisabled(true);
    _controls.BTN_POWERS->setDisabled(true);
    _controls.BTN_FEATS->setDisabled(true);

    _controls.LBL_SKILLRANK->setTextMessage(_services.resource.strings.getText(kStrRefSkillRank));
    _controls.LBL_BONUS->setTextMessage(_services.resource.strings.getText(kStrRefBonus));
    _controls.LBL_TOTAL->setTextMessage(_services.resource.strings.getText(kStrRefTotalRank));
    _controls.LBL_RANKVAL->setTextMessage("");
    _controls.LBL_BONUSVAL->setTextMessage("");
    _controls.LBL_TOTALVAL->setTextMessage("");
    _controls.LBL_NAME->setTextMessage("");

    _controls.LB_DESC->setProtoMatchContent(true);
    _controls.LB_ABILITY->setOnItemClick([this](const std::string &item) {
        auto skill = static_cast<SkillType>(stoi(item));
        auto maybeSkillInfo = _skills.find(skill);
        if (maybeSkillInfo == _skills.end())
            return;

        std::shared_ptr<Creature> partyLeader(_game.party().getLeader());

        _controls.LBL_RANKVAL->setTextMessage(std::to_string(partyLeader->attributes().getSkillRank(skill)));
        _controls.LBL_BONUSVAL->setTextMessage("0");
        _controls.LBL_TOTALVAL->setTextMessage(std::to_string(partyLeader->attributes().getSkillRank(skill)));
        _controls.LBL_NAME->setTextMessage(maybeSkillInfo->second.name);

        _controls.LB_DESC->clearItems();
        _controls.LB_DESC->addTextLinesAsItems(maybeSkillInfo->second.description);
    });
    _controls.BTN_EXIT->setOnClick([this]() {
        _game.openInGame();
    });

    loadSkills();
}

void AbilitiesMenu::loadSkills() {
    std::shared_ptr<TwoDa> skills(_services.resource.twoDas.get("skills"));
    for (int row = 0; row < skills->getRowCount(); ++row) {
        auto skill = static_cast<SkillType>(row);

        SkillInfo skillInfo;
        skillInfo.skill = skill;
        skillInfo.name = _services.resource.strings.getText(skills->getInt(row, "name"));
        skillInfo.description = _services.resource.strings.getText(skills->getInt(row, "description"));
        skillInfo.icon = _services.resource.textures.get(skills->getString(row, "icon"), TextureUsage::GUI);

        _skills.insert(std::make_pair(skill, std::move(skillInfo)));
    }

    _controls.LB_ABILITY->clearItems();
    for (auto &skill : _skills) {
        ListBox::Item item;
        item.tag = std::to_string(static_cast<int>(skill.second.skill));
        item.text = skill.second.name;
        item.iconFrame = getFrameTexture();
        item.iconTexture = skill.second.icon;
        _controls.LB_ABILITY->addItem(std::move(item));
    }
}

std::shared_ptr<Texture> AbilitiesMenu::getFrameTexture() const {
    std::string resRef;
    if (_game.isTSL()) {
        resRef = "uibit_eqp_itm1";
    } else {
        resRef = "lbl_hex_3";
    }
    return _services.resource.textures.get(resRef, TextureUsage::GUI);
}

void AbilitiesMenu::refreshControls() {
    refreshPortraits();
}

void AbilitiesMenu::refreshPortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> partyLeader(party.getLeader());
    std::shared_ptr<Creature> partyMember1(party.getMember(1));
    std::shared_ptr<Creature> partyMember2(party.getMember(2));

    _controls.BTN_CHANGE1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _controls.BTN_CHANGE1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _controls.BTN_CHANGE2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _controls.BTN_CHANGE2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

} // namespace game

} // namespace reone
