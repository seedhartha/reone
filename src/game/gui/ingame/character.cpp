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

#include "character.h"

#include "../../game.h"

#include "../colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CharacterMenu::CharacterMenu(Game *game) :
    GameGUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("character");
    _backgroundType = BackgroundType::Menu;

    initForGame();
}

void CharacterMenu::load() {
    GUI::load();

    hideControl("BTN_CHARLEFT");
    hideControl("BTN_CHARRIGHT");
    hideControl("LBL_GOOD0");
    hideControl("LBL_GOOD1");
    hideControl("LBL_GOOD2");
    hideControl("LBL_GOOD3");
    hideControl("LBL_GOOD4");
    hideControl("LBL_GOOD5");
    hideControl("LBL_GOOD6");
    hideControl("LBL_GOOD7");
    hideControl("LBL_GOOD8");
    hideControl("LBL_GOOD9");
    hideControl("LBL_GOOD10");
    hideControl("LBL_MORE");

    hideControl("LBL_CLASS1");
    hideControl("LBL_CLASS2");
    hideControl("LBL_LEVEL1");
    hideControl("LBL_LEVEL2");

    hideControl("LBL_VITALITY_STAT");
    hideControl("LBL_DEFENSE_STAT");
    hideControl("LBL_FORCE_STAT");

    hideControl("LBL_STR");
    hideControl("LBL_STR_MOD");
    hideControl("LBL_DEX");
    hideControl("LBL_DEX_MOD");
    hideControl("LBL_CON");
    hideControl("LBL_CON_MOD");
    hideControl("LBL_INT");
    hideControl("LBL_INT_MOD");
    hideControl("LBL_WIS");
    hideControl("LBL_WIS_MOD");
    hideControl("LBL_CHA");
    hideControl("LBL_CHA_MOD");

    hideControl("LBL_FORTITUDE_STAT");
    hideControl("LBL_REFLEX_STAT");
    hideControl("LBL_WILL_STAT");

    hideControl("LBL_EXPERIENCE_STAT");
    hideControl("LBL_NEEDED_XP");

    disableControl("BTN_AUTO");
    disableControl("BTN_SCRIPTS");
}

void CharacterMenu::update(float dt) {
    shared_ptr<Creature> leader(_game->party().leader());
    setControlVisible("BTN_LEVELUP", leader->isLevelUpPending());
    setControlVisible("BTN_AUTO", leader->isLevelUpPending());
}

void CharacterMenu::refreshPortraits() {
    if (_version != GameVersion::KotOR) return;

    Party &party = _game->party();
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    Control &btnChange1 = getControl("BTN_CHANGE1");
    btnChange1.setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);

    Control &btnChange2 = getControl("BTN_CHANGE2");
    btnChange2.setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::onClick(const string &control) {
    if (control == "BTN_EXIT") {
        _game->openInGame();
    } else if (control == "BTN_LEVELUP") {
        _game->openLevelUp();
    }
}

} // namespace game

} // namespace reone
