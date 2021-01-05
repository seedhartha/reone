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

#include "partyselect.h"

#include <boost/algorithm/string.hpp>

#include "../../gui/control/label.h"
#include "../../gui/control/togglebutton.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"
#include "../../script/types.h"

#include "../blueprint/blueprints.h"
#include "../game.h"
#include "../portraitutil.h"

#include "colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

static const int kMaxFollowerCount = 2;

static int g_strRefAdd = 38455;
static int g_strRefRemove = 38456;

static glm::vec3 g_kotorColorOn = { 0.984314f, 1.0f, 0 };
static glm::vec3 g_kotorColorAdded = { 0, 0.831373f, 0.090196f };

static map<int, string> g_portraitByAppearance = {
    { 378, "po_ptrask"}
};

PartySelection::PartySelection(Game *game) :
    GameGUI(game->version(), game->options().graphics),
    _game(game) {

    if (_version == GameVersion::TheSithLords) {
        _resRef = "partyselect_p";
    } else {
        _resRef = "partyselection";
        _backgroundType = BackgroundType::Menu;
    }

    initForGame();
}

void PartySelection::load() {
    GUI::load();

    for (int i = 0; i < kNpcCount; ++i) {
        ToggleButton &button = getNpcButton(i);
        button.setOnColor(g_kotorColorOn);
        button.setBorderColorOverride(g_kotorColorAdded);
        button.setUseBorderColorOverride(false);
    }
}

void PartySelection::prepare(const Context &ctx) {
    _context = ctx;
    _availableCount = kMaxFollowerCount;

    for (int i = 0; i < kNpcCount; ++i) {
        _added[i] = false;
        getNpcButton(i).setUseBorderColorOverride(false);
    }
    if (ctx.forceNpc1 >= 0) {
        addNpc(ctx.forceNpc1);
    }
    if (ctx.forceNpc2 >= 0) {
        addNpc(ctx.forceNpc2);
    }
    Party &party = _game->party();

    for (int i = 0; i < kNpcCount; ++i) {
        ToggleButton &btnNpc = getNpcButton(i);
        Label &lblChar = getControl<Label>("LBL_CHAR" + to_string(i));
        Label &lblNa = getControl<Label>("LBL_NA" + to_string(i));

        if (party.isMemberAvailable(i)) {
            string blueprintResRef(party.getAvailableMember(i));
            shared_ptr<CreatureBlueprint> blueprint(Blueprints::instance().getCreature(blueprintResRef));
            int appearance = blueprint->getAppearanceFromUtc();
            string portrait;

            auto maybePortrait = g_portraitByAppearance.find(appearance);
            if (maybePortrait != g_portraitByAppearance.end()) {
                portrait = maybePortrait->second;
            } else {
                portrait = getPortraitByAppearance(blueprint->getAppearanceFromUtc());
            }
            btnNpc.setDisabled(false);
            lblChar.setBorderFill(Textures::instance().get(portrait, TextureType::GUI));
            lblNa.setVisible(false);

        } else {
            btnNpc.setDisabled(true);
            lblChar.setBorderFill(shared_ptr<Texture>(nullptr));
            lblNa.setVisible(true);
        }
    }
    refreshAvailableCount();
}

void PartySelection::addNpc(int npc) {
    --_availableCount;
    _added[npc] = true;
    getNpcButton(npc).setUseBorderColorOverride(true);
    refreshAvailableCount();
}

ToggleButton &PartySelection::getNpcButton(int npc) {
    string tag("BTN_NPC" + to_string(npc));
    return getControl<ToggleButton>(tag);
}

void PartySelection::onClick(const string &control) {
    if (control == "BTN_ACCEPT") {
        onAcceptButtonClick();

    } else if (control == "BTN_DONE" || control == "BTN_BACK") {
        changeParty();
        _game->openInGame();

        if (!_context.exitScript.empty()) {
            _game->scriptRunner().run(_context.exitScript);
        }
    } else if (boost::starts_with(control, "BTN_NPC")) {
        onNpcButtonClick(control);
    }
}

void PartySelection::onAcceptButtonClick() {
    if (_selectedNpc == -1) return;

    bool added = _added[_selectedNpc];
    if (added && _context.forceNpc1 != _selectedNpc && _context.forceNpc2 != _selectedNpc) {
        removeNpc(_selectedNpc);
        refreshAcceptButton();

    } else if (!added && _availableCount > 0) {
        addNpc(_selectedNpc);
        refreshAcceptButton();
    }
}

void PartySelection::refreshAvailableCount() {
    Label &label = getControl<Label>("LBL_COUNT");
    label.setTextMessage(to_string(_availableCount));
}

void PartySelection::refreshAcceptButton() {
    string text(Resources::instance().getString(_added[_selectedNpc] ? g_strRefRemove : g_strRefAdd));
    Button &btnAccept = getControl<Button>("BTN_ACCEPT");
    btnAccept.setTextMessage(text);
}

void PartySelection::removeNpc(int npc) {
    ++_availableCount;
    _added[npc] = false;
    getNpcButton(npc).setUseBorderColorOverride(false);
    refreshAvailableCount();
}

void PartySelection::onNpcButtonClick(const string &control) {
    int npc = control.substr(7, 1)[0] - '0';
    _selectedNpc = npc;
    refreshNpcButtons();
    refreshAcceptButton();
}

void PartySelection::refreshNpcButtons() {
    for (int i = 0; i < kNpcCount; ++i) {
        ToggleButton &button = getNpcButton(i);

        if ((i == _selectedNpc && !button.isOn()) ||
            (i != _selectedNpc && button.isOn())) {

            button.toggle();
        }
    }
}

void PartySelection::changeParty() {
    shared_ptr<Area> area(_game->module()->area());
    area->unloadParty();

    Party &party = _game->party();
    party.clear();
    party.addMember(kNpcPlayer, party.player());

    shared_ptr<Creature> player(_game->party().player());

    for (int i = 0; i < kNpcCount; ++i) {
        if (!_added[i]) continue;

        string blueprintResRef(party.getAvailableMember(i));
        shared_ptr<CreatureBlueprint> blueprint(Blueprints::instance().getCreature(blueprintResRef));

        shared_ptr<Creature> creature(_game->objectFactory().newCreature());
        creature->load(blueprint);
        creature->setFaction(Faction::Friendly1);
        creature->setImmortal(true);
        creature->actionQueue().add(make_unique<FollowAction>(player, 1.0f));
        party.addMember(i, creature);
    }

    area->reloadParty();
}

} // namespace game

} // namespace reone
