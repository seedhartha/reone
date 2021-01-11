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

#include "../../game.h"

#include "../colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

AbilitiesMenu::AbilitiesMenu(Game *game) :
    GameGUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("abilities");
    _backgroundType = BackgroundType::Menu;

    initForGame();
}

void AbilitiesMenu::load() {
    GUI::load();

    hideControl("BTN_CHARLEFT");
    hideControl("BTN_CHARRIGHT");
    hideControl("LBL_RANKVAL");
    hideControl("LBL_BONUSVAL");
    hideControl("LBL_TOTALVAL");
    hideControl("LBL_NAME");

    disableControl("BTN_SKILLS");
    disableControl("BTN_POWERS");
    disableControl("BTN_FEATS");
}

void AbilitiesMenu::refreshPortraits() {
    if (_version != GameVersion::KotOR) return;

    Party &party = _game->party();
    shared_ptr<Creature> partyLeader(party.leader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    Control &btnChange1 = getControl("BTN_CHANGE1");
    btnChange1.setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);

    Control &btnChange2 = getControl("BTN_CHANGE2");
    btnChange2.setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void AbilitiesMenu::onClick(const string &control) {
    if (control == "BTN_EXIT") {
        _game->openInGame();
    }
}

} // namespace game

} // namespace reone
