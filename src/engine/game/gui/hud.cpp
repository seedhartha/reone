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

#include "hud.h"

#include "../../common/log.h"
#include "../../gui/control/label.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/window.h"

#include "../game.h"
#include "../gameidutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

HUD::HUD(Game *game) : GameGUI(game), _select(game) {
    _resRef = getResRef("mipc28x6");
    _resolutionX = 800;
    _resolutionY = 600;
    _scaling = ScalingMode::PositionRelativeToCenter;

    static string combatControlTags[] = {
        "BTN_CLEARALL", "BTN_CLEARONE", "BTN_CLEARONE2",
        "LBL_CMBTMODEMSG", "LBL_CMBTMSGBG", "LBL_COMBATBG1", "LBL_COMBATBG2", "LBL_COMBATBG3",
        "LBL_QUEUE0", "LBL_QUEUE1", "LBL_QUEUE2", "LBL_QUEUE3"
    };
    for (auto &tag : combatControlTags) {
        _scalingByControlTag.insert(make_pair(tag, ScalingMode::Stretch));
    }
}

void HUD::load() {
    GUI::load();

    _select.load();

    _barkBubble = make_unique<BarkBubble>(_game);
    _barkBubble->load();

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
    hideControl("LBL_LIGHTSHIFT");
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
            setControlVisible("LBL_LVLUPBG" + to_string(charIdx), member->isLevelUpPending());
            setControlVisible("LBL_LEVELUP" + to_string(charIdx), member->isLevelUpPending());
        } else {
            lblChar.setVisible(false);
            lblBack.setVisible(false);
            hideControl("LBL_LVLUPBG" + to_string(charIdx));
            hideControl("LBL_LEVELUP" + to_string(charIdx));
        }
    }

    if (party.getLeader()->isInCombat()) {
        showCombatHud();
        refreshActionQueueItems();
    } else {
        hideCombatHud();
    }

    _select.update();
    _barkBubble->update(dt);

    // Hide minimap when there is no image to display
    setControlVisible("LBL_MAPBORDER", _game->module()->area()->map().isLoaded());
}

void HUD::draw() {
    GUI::draw();

    drawMinimap();

    Party &party = _game->party();
    for (int i = 0; i < party.getSize(); ++i) {
        drawHealth(i);
    }

    _barkBubble->draw();
    _select.draw();
}

void HUD::drawMinimap() {
    Control &label = getControl("LBL_MAPVIEW");
    const Control::Extent &extent = label.extent();

    glm::vec4 bounds;
    bounds[0] = static_cast<float>(_controlOffset.x + extent.left);
    bounds[1] = static_cast<float>(_controlOffset.y + extent.top);
    bounds[2] = static_cast<float>(extent.width);
    bounds[3] = static_cast<float>(extent.height);

    shared_ptr<Area> area(_game->module()->area());
    area->map().draw(Map::Mode::Minimap, bounds);
}

void HUD::drawHealth(int memberIndex) {
    if (isTSL(_game->gameId())) return;

    Party &party = _game->party();
    shared_ptr<Creature> member(party.getMember(memberIndex));

    int charIdx = (memberIndex == 0) ? 1 : (4 - memberIndex);
    Control &lblChar = getControl("LBL_BACK" + to_string(charIdx));
    const Control::Extent &extent = lblChar.extent();

    float w = 5.0f;
    float h = glm::clamp(member->currentHitPoints() / static_cast<float>(member->hitPoints()), 0.0f, 1.0f) * extent.height;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_controlOffset.x + extent.left + extent.width - 14.0f, _controlOffset.y + extent.top + extent.height - h, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _game->window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);
    uniforms.combined.general.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    Shaders::instance().activate(ShaderProgram::SimpleColor, uniforms);
    Meshes::instance().getQuad()->draw();
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

void HUD::refreshActionQueueItems() const {
    auto &actions = _game->party().getLeader()->actions();

    for (int i = 0; i < 4; ++i) {
        bool attack = i < static_cast<int>(actions.size()) && actions[i]->type() == ActionType::AttackObject;
        Control &item = getControl("LBL_QUEUE" + to_string(i));
        item.setBorderFill(attack ? "i_attack" : "");
    }
}

void HUD::onClick(const string &control) {
    GameGUI::onClick(control);

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
        _game->party().getLeader()->clearAllActions();
    } else if (control == "BTN_CLEARONE" || control == "BTN_CLEARONE2") {
        for (auto &action : _game->party().getLeader()->actions()) {
            if (action->type() == ActionType::AttackObject) {
                action->complete();
                break;
            }
        }
    } else if (control == "BTN_CHAR1") {
        _game->openInGameMenu(InGameMenu::Tab::Equipment);
    } else if (control == "BTN_CHAR2" || control == "BTN_CHAR3") {
        int memberIdx = 4 - stoi(&control[8]);
        _game->party().setPartyLeaderByIndex(memberIdx);
    }
}

} // namespace game

} // namespace reone
