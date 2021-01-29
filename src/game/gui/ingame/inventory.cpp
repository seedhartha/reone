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

#include "../colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

InventoryMenu::InventoryMenu(Game *game) :
    GameGUI(game->gameId(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("inventory");
    _backgroundType = BackgroundType::Menu;

    initForGame();
}

void InventoryMenu::load() {
    GUI::load();

    hideControl("BTN_CHARLEFT");
    hideControl("BTN_CHARRIGHT");
    hideControl("LBL_CREDITS_VALUE");
    hideControl("LBL_VIT");
    hideControl("LBL_DEF");

    disableControl("BTN_USEITEM");
    disableControl("BTN_QUESTITEMS");

    setControlFocusable("BTN_CHANGE1", false);
    setControlFocusable("BTN_CHANGE2", false);
}

void InventoryMenu::refreshPortraits() {
    if (_gameId != GameID::KotOR) return;

    Party &party = _game->party();
    shared_ptr<Creature> partyLeader(party.leader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    Control &lblPortrait = getControl("LBL_PORT");
    lblPortrait.setBorderFill(partyLeader->portrait());

    Control &btnChange1 = getControl("BTN_CHANGE1");
    btnChange1.setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);

    Control &btnChange2 = getControl("BTN_CHANGE2");
    btnChange2.setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void InventoryMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    }
}

} // namespace game

} // namespace reone
