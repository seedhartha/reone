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
#include "../../rp/classes.h"

#include "../colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CharacterMenu::CharacterMenu(Game *game) :
    GameGUI(game->gameId(), game->options().graphics),
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

    disableControl("BTN_AUTO");
    disableControl("BTN_SCRIPTS");
}

void CharacterMenu::update(float dt) {
    shared_ptr<Creature> leader(_game->party().getLeader());
    setControlVisible("BTN_LEVELUP", leader->isLevelUpPending());
    setControlVisible("BTN_AUTO", leader->isLevelUpPending());
}

static string describeClass(ClassType clazz) {
    if (clazz == ClassType::Invalid) return "";

    return Classes::instance().get(clazz)->name();
}

static string toStringOrEmptyIfZero(int value) {
    return value != 0 ? to_string(value) : "";
}

void CharacterMenu::refreshControls() {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    CreatureAttributes &attributes = partyLeader->attributes();

    setControlText("LBL_CLASS1", describeClass(attributes.getClassByPosition(1)));
    setControlText("LBL_CLASS2", describeClass(attributes.getClassByPosition(2)));
    setControlText("LBL_LEVEL1", toStringOrEmptyIfZero(attributes.getLevelByPosition(1)));
    setControlText("LBL_LEVEL2", toStringOrEmptyIfZero(attributes.getLevelByPosition(2)));

    setControlText("LBL_VITALITY_STAT", "");
    setControlText("LBL_DEFENSE_STAT", "");
    setControlText("LBL_FORCE_STAT", "");

    setControlText("LBL_STR", "");
    setControlText("LBL_STR_MOD", "");
    setControlText("LBL_DEX", "");
    setControlText("LBL_DEX_MOD", "");
    setControlText("LBL_CON", "");
    setControlText("LBL_CON_MOD", "");
    setControlText("LBL_INT", "");
    setControlText("LBL_INT_MOD", "");
    setControlText("LBL_WIS", "");
    setControlText("LBL_WIS_MOD", "");
    setControlText("LBL_CHA", "");
    setControlText("LBL_CHA_MOD", "");

    setControlText("LBL_FORTITUDE_STAT", "");
    setControlText("LBL_REFLEX_STAT", "");
    setControlText("LBL_WILL_STAT", "");

    setControlText("LBL_EXPERIENCE_STAT", "");
    setControlText("LBL_NEEDED_XP", "");

    refreshPortraits();
}

void CharacterMenu::refreshPortraits() {
    if (_gameId != GameID::KotOR) return;

    Party &party = _game->party();
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    Control &btnChange1 = getControl("BTN_CHANGE1");
    btnChange1.setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);

    Control &btnChange2 = getControl("BTN_CHANGE2");
    btnChange2.setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    } else if (control == "BTN_LEVELUP") {
        _game->openLevelUp();
    }
}

} // namespace game

} // namespace reone
