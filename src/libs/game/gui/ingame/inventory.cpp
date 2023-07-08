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

#include "reone/game/gui/ingame/inventory.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/party.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void InventoryMenu::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    _binding.lblCreditsValue->setVisible(false);
    _binding.btnUseItem->setDisabled(true);
    _binding.btnExit->setOnClick([this]() {
        _game.openInGame();
    });

    if (!_game.isTSL()) {
        _binding.lblVit->setVisible(false);
        _binding.lblDef->setVisible(false);
        _binding.btnChange1->setFocusable(false);
        _binding.btnChange2->setFocusable(false);
        _binding.btnQuestItems->setDisabled(true);
    }
}

void InventoryMenu::bindControls() {
    _binding.btnExit = findControl<Button>("BTN_EXIT");
    _binding.btnUseItem = findControl<Button>("BTN_USEITEM");
    _binding.lblCredits = findControl<Label>("LBL_CREDITS");
    _binding.lblCreditsValue = findControl<Label>("LBL_CREDITS_VALUE");
    _binding.lblInv = findControl<Label>("LBL_INV");
    _binding.lbDescription = findControl<ListBox>("LB_DESCRIPTION");
    _binding.lbItems = findControl<ListBox>("LB_ITEMS");

    if (!_game.isTSL()) {
        _binding.btnChange1 = findControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = findControl<Button>("BTN_CHANGE2");
        _binding.btnQuestItems = findControl<Button>("BTN_QUESTITEMS");
        _binding.lblBgPort = findControl<Label>("LBL_BGPORT");
        _binding.lblBgStats = findControl<Label>("LBL_BGSTATS");
        _binding.lblDef = findControl<Label>("LBL_DEF");
        _binding.lblPort = findControl<Label>("LBL_PORT");
        _binding.lblVit = findControl<Label>("LBL_VIT");
    } else {
        _binding.btnAll = findControl<Button>("BTN_ALL");
        _binding.btnArmor = findControl<Button>("BTN_ARMOR");
        _binding.btnDatapads = findControl<Button>("BTN_DATAPADS");
        _binding.btnMisc = findControl<Button>("BTN_MISC");
        _binding.btnQuests = findControl<Button>("BTN_QUESTS");
        _binding.btnUseable = findControl<Button>("BTN_USEABLE");
        _binding.btnWeapons = findControl<Button>("BTN_WEAPONS");
        _binding.lblBar1 = findControl<Label>("LBL_BAR1");
        _binding.lblBar2 = findControl<Label>("LBL_BAR2");
        _binding.lblBar3 = findControl<Label>("LBL_BAR3");
        _binding.lblBar4 = findControl<Label>("LBL_BAR4");
        _binding.lblBar5 = findControl<Label>("LBL_BAR5");
        _binding.lblBar6 = findControl<Label>("LBL_BAR6");
        _binding.lblFilter = findControl<Label>("LBL_FILTER");
    }
}

void InventoryMenu::refreshPortraits() {
    if (!!_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> partyLeader(party.getLeader());
    std::shared_ptr<Creature> partyMember1(party.getMember(1));
    std::shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.lblPort->setBorderFill(partyLeader->portrait());

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

} // namespace game

} // namespace reone
