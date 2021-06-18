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

#include "inventory.h"

#include "../../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

InventoryMenu::InventoryMenu(Game *game) : GameGUI(game) {
    _resRef = getResRef("inventory");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void InventoryMenu::load() {
    GUI::load();
    bindControls();

    _binding.lblCreditsValue->setVisible(false);
    _binding.btnUseItem->setDisabled(true);
    _binding.btnQuestItems->setDisabled(true);

    if (_game->isKotOR()) {
        _binding.lblVit->setVisible(false);
        _binding.lblDef->setVisible(false);
        _binding.btnChange1->setFocusable(false);
        _binding.btnChange2->setFocusable(false);
    }
}

void InventoryMenu::bindControls() {
    _binding.btnExit = getControlPtr<Button>("BTN_EXIT");
    _binding.btnQuestItems = getControlPtr<Button>("BTN_QUESTITEMS");
    _binding.btnUseItem = getControlPtr<Button>("BTN_USEITEM");
    _binding.lblCredits = getControlPtr<Label>("LBL_CREDITS");
    _binding.lblCreditsValue = getControlPtr<Label>("LBL_CREDITS_VALUE");
    _binding.lblInv = getControlPtr<Label>("LBL_INV");
    _binding.lbDescription = getControlPtr<ListBox>("LB_DESCRIPTION");
    _binding.lbItems = getControlPtr<ListBox>("LB_ITEMS");

    if (_game->isKotOR()) {
        _binding.btnChange1 = getControlPtr<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControlPtr<Button>("BTN_CHANGE2");
        _binding.lblBgPort = getControlPtr<Label>("LBL_BGPORT");
        _binding.lblBgStats = getControlPtr<Label>("LBL_BGSTATS");
        _binding.lblDef = getControlPtr<Label>("LBL_DEF");
        _binding.lblPort = getControlPtr<Label>("LBL_PORT");
        _binding.lblVit = getControlPtr<Label>("LBL_VIT");
    } else {
        _binding.btnAll = getControlPtr<Button>("BTN_ALL");
        _binding.btnArmor = getControlPtr<Button>("BTN_ARMOR");
        _binding.btnDatapads = getControlPtr<Button>("BTN_DATAPADS");
        _binding.btnMisc = getControlPtr<Button>("BTN_MISC");
        _binding.btnQuests = getControlPtr<Button>("BTN_QUESTS");
        _binding.btnUseable = getControlPtr<Button>("BTN_USEABLE");
        _binding.btnWeapons = getControlPtr<Button>("BTN_WEAPONS");
        _binding.lblBar1 = getControlPtr<Label>("LBL_BAR1");
        _binding.lblBar2 = getControlPtr<Label>("LBL_BAR2");
        _binding.lblBar3 = getControlPtr<Label>("LBL_BAR3");
        _binding.lblBar4 = getControlPtr<Label>("LBL_BAR4");
        _binding.lblBar5 = getControlPtr<Label>("LBL_BAR5");
        _binding.lblBar6 = getControlPtr<Label>("LBL_BAR6");
        _binding.lblFilter = getControlPtr<Label>("LBL_FILTER");
    }
}

void InventoryMenu::refreshPortraits() {
    if (_game->id() != GameID::KotOR) return;

    Party &party = _game->services().party();
    shared_ptr<Creature> partyLeader(party.getLeader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.lblPort->setBorderFill(partyLeader->portrait());

    if (_game->isKotOR()) {
        _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
        _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

        _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
        _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
    }
}

void InventoryMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    }
}

} // namespace game

} // namespace reone
