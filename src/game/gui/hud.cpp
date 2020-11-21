/*
 * Copyright (c) 2020 The reone project contributors
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

#include "hud.h"

#include "../../gui/control/label.h"
#include "../../common/log.h"

#include "../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

HUD::HUD(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game),
    _debug(game->options().graphics),
    _select(game) {

    if (!game) {
        throw invalid_argument("Game must not be null");
    }
    _resRef = getResRef("mipc28x6");
    _resolutionX = 800;
    _resolutionY = 600;
    _scaling = ScalingMode::PositionRelativeToCenter;
}

void HUD::load() {
    GUI::load();
    _debug.load();
    _select.load();

    hideControl("BTN_CLEARALL");
    hideControl("BTN_SWAPWEAPONS");
    hideControl("BTN_TARGET0");
    hideControl("BTN_TARGET1");
    hideControl("BTN_TARGET2");
    hideControl("BTN_TARGETDOWN0");
    hideControl("BTN_TARGETDOWN1");
    hideControl("BTN_TARGETDOWN2");
    hideControl("BTN_TARGETUP0");
    hideControl("BTN_TARGETUP1");
    hideControl("BTN_TARGETUP2");
    hideControl("LBL_ACTIONDESCBG");
    hideControl("LBL_ACTIONTYPE0");
    hideControl("LBL_ACTIONTYPE1");
    hideControl("LBL_ACTIONTYPE2");
    hideControl("LBL_ACTIONTYPE3");
    hideControl("LBL_ACTIONTYPE4");
    hideControl("LBL_ACTIONTYPE5");
    hideControl("LBL_ACTIONDESC");
    hideControl("LBL_ARROW_MARGIN");
    hideControl("LBL_CASH");
    hideControl("LBL_CMBTEFCTINC1");
    hideControl("LBL_CMBTEFCTINC2");
    hideControl("LBL_CMBTEFCTINC3");
    hideControl("LBL_CMBTEFCTRED1");
    hideControl("LBL_CMBTEFCTRED2");
    hideControl("LBL_CMBTEFCTRED3");
    hideControl("LBL_CMBTMODEMSG");
    hideControl("LBL_CMBTMSGBG");
    hideControl("LBL_COMBATBG1");
    hideControl("LBL_COMBATBG2");
    hideControl("LBL_COMBATBG3");
    hideControl("LBL_DARKSHIFT");
    hideControl("LBL_DEBILATATED1");
    hideControl("LBL_DEBILATATED2");
    hideControl("LBL_DEBILATATED3");
    hideControl("LBL_DISABLE1");
    hideControl("LBL_DISABLE2");
    hideControl("LBL_DISABLE3");
    hideControl("LBL_JOURNAL");
    hideControl("LBL_HEALTHBG");
    hideControl("LBL_INDICATE");
    hideControl("LBL_INDICATEBG");
    hideControl("LBL_ITEMRCVD");
    hideControl("LBL_ITEMLOST");
    hideControl("LBL_LEVELUP1");
    hideControl("LBL_LEVELUP2");
    hideControl("LBL_LEVELUP3");
    hideControl("LBL_LIGHTSHIFT");
    hideControl("LBL_LVLUPBG1");
    hideControl("LBL_LVLUPBG2");
    hideControl("LBL_LVLUPBG3");
    hideControl("LBL_MAP");
    hideControl("LBL_MOULDING1");
    hideControl("LBL_MOULDING2");
    hideControl("LBL_MOULDING3");
    hideControl("LBL_MOULDING4");
    hideControl("LBL_NAME");
    hideControl("LBL_NAMEBG");
    hideControl("LBL_PLOTXP");
    hideControl("LBL_STEALTHXP");

    hideControl("BTN_ACTION0");
    hideControl("BTN_ACTION1");
    hideControl("BTN_ACTION2");
    hideControl("BTN_ACTION3");
    hideControl("BTN_ACTION4");
    hideControl("BTN_ACTION5");
    hideControl("BTN_ACTIONDOWN0");
    hideControl("BTN_ACTIONDOWN1");
    hideControl("BTN_ACTIONDOWN2");
    hideControl("BTN_ACTIONDOWN3");
    hideControl("BTN_ACTIONDOWN4");
    hideControl("BTN_ACTIONDOWN5");
    hideControl("BTN_ACTIONUP0");
    hideControl("BTN_ACTIONUP1");
    hideControl("BTN_ACTIONUP2");
    hideControl("BTN_ACTIONUP3");
    hideControl("BTN_ACTIONUP4");
    hideControl("BTN_ACTIONUP5");
    hideControl("BTN_MINIMAP");
    hideControl("LBL_ACTION0");
    hideControl("LBL_ACTION1");
    hideControl("LBL_ACTION2");
    hideControl("LBL_ACTION3");
    hideControl("LBL_ACTION4");
    hideControl("LBL_ACTION5");
    hideControl("LBL_ARROW");
    hideControl("LBL_MAPBORDER");
    hideControl("LBL_MAPVIEW");
    hideControl("LBL_TARGET0");
    hideControl("TB_PAUSE");
    hideControl("TB_SOLO");
    hideControl("TB_STEALTH");
}

bool HUD::handle(const SDL_Event &event) {
    if (_select.handle(event)) return true;

    return GUI::handle(event);
}

void HUD::update(float dt) {
    GUI::update(dt);

    Party &party = _game->party();

    for (int i = 0; i < 3; ++i) {
        int charIdx = (i == 0) ? 1 : (4 - i);
        Control &lblChar = getControl("LBL_CHAR" + to_string(charIdx));
        Control &lblBack = getControl("LBL_BACK" + to_string(charIdx));

        shared_ptr<Creature> member(party.getMember(i));
        if (member) {
            lblChar.setVisible(true);
            lblChar.setBorderFill(member->portrait());
            lblBack.setVisible(true);
        } else {
            lblChar.setVisible(false);
            lblBack.setVisible(false);
        }
    }

    if (_game->module()->area()->combat().isActive()) {
        showCombatHud();
        drawActionQueueItems();
    }
    else {
        hideCombatHud();
    }

    _select.update();
}

void HUD::render() const {
    GUI::render();
    _select.render();
    _debug.render();
}

void HUD::showCombatHud() {
    // TODO: make those class members?
    showControl("BTN_CLEARALL");
    showControl("BTN_CLEARONE");
    showControl("BTN_CLEARONE2");

    showControl("LBL_CMBTMODEMSG");
    showControl("LBL_CMBTMSGBG");

    showControl("LBL_COMBATBG1");
    showControl("LBL_COMBATBG2");
    showControl("LBL_COMBATBG3");

    showControl("LBL_QUEUE0");
    showControl("LBL_QUEUE1");
    showControl("LBL_QUEUE2");
    showControl("LBL_QUEUE3");
}

void HUD::hideCombatHud() {
    hideControl("BTN_CLEARALL");
    hideControl("BTN_CLEARONE");
    showControl("BTN_CLEARONE2");

    hideControl("LBL_CMBTMODEMSG");
    hideControl("LBL_CMBTMSGBG");

    hideControl("LBL_COMBATBG1");
    hideControl("LBL_COMBATBG2");
    hideControl("LBL_COMBATBG3");

    hideControl("LBL_QUEUE0");
    hideControl("LBL_QUEUE1");
    hideControl("LBL_QUEUE2");
    hideControl("LBL_QUEUE3");
}

void HUD::drawActionQueueItems() const {
    auto &actionQueue = _game->party().leader()->actionQueue();

    // clear the drawn queue items first
    for (int j = 0; j < 4; ++j) {
        Control &qItem = getControl("LBL_QUEUE" + to_string(j));
        qItem.setBorderFill("");
    }

    auto it = actionQueue.begin();
    int i = 0;
    while (i < 4 && it != actionQueue.end()) {
        ActionType actionType = (*it)->type();

        // TODO: if (isDisplayableAction(*it))
        if (actionType == ActionType::AttackObject) {
            Control &qItem = getControl("LBL_QUEUE"+to_string(i));
            qItem.setBorderFill("i_attack");
            ++i;
        }

        it += 1;
    }
}

void HUD::onClick(const string &control) {
    if (control == "BTN_EQU") {
        _game->openInGameMenu(InGameMenu::Tab::Equipment);
    } else if (control == "BTN_INV") {
        _game->openInGameMenu(InGameMenu::Tab::Inventory);
    } else if (control == "BTN_CHAR") {
        _game->openInGameMenu(InGameMenu::Tab::Character);
    } else if (control == "BTN_ABI") {
        _game->openInGameMenu(InGameMenu::Tab::Abilities);
    } else if (control == "BTN_MSG") {
        _game->openInGameMenu(InGameMenu::Tab::Messages);
    } else if (control == "BTN_JOU") {
        _game->openInGameMenu(InGameMenu::Tab::Journal);
    } else if (control == "BTN_MAP") {
        _game->openInGameMenu(InGameMenu::Tab::Map);
    } else if (control == "BTN_OPT") {
        _game->openInGameMenu(InGameMenu::Tab::Options);
    } else if (control == "BTN_CLEARALL") {
        _game->party().leader()->actionQueue().clear();
    } else if (control == "BTN_CLEARONE" || control == "BTN_CLEARONE2") {
        auto &actionQueue = _game->party().leader()->actionQueue();

        for (auto it=actionQueue.begin(); it!=actionQueue.end(); ++it) {

            // TODO: if (isDisplayableAction(*it))
            if ((*it)->type() == ActionType::AttackObject) {
                (*it)->complete();
                break;
            }
        }
    }
}

} // namespace game

} // namespace reone
