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

#include "reone/game/gui/partyselect.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/party.h"
#include "reone/game/portrait.h"
#include "reone/game/portraits.h"
#include "reone/game/script/runner.h"
#include "reone/graphics/textures.h"
#include "reone/resource/di/services.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"
#include "reone/script/types.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr int kMaxFollowerCount = 2;

static int g_strRefAdd = 38455;
static int g_strRefRemove = 38456;

static glm::vec3 g_kotorColorOn = {0.984314f, 1.0f, 0};
static glm::vec3 g_kotorColorAdded = {0, 0.831373f, 0.090196f};

PartySelection::PartySelection(Game &game, ServicesView &services) :
    GameGUI(game, services) {

    if (game.isTSL()) {
        _resRef = "partyselect_p";
    } else {
        _resRef = "partyselection";
    }
}

void PartySelection::onGUILoaded() {
    if (!_game.isTSL()) {
        loadBackground(BackgroundType::Menu);
    }

    bindControls();

    for (int i = 0; i < kNpcCount; ++i) {
        ToggleButton &button = getNpcButton(i);
        button.setOnColor(g_kotorColorOn);
        button.setBorderColorOverride(g_kotorColorAdded);
        button.setUseBorderColorOverride(false);
    }

    _controls.BTN_ACCEPT->setOnClick([this]() {
        onAcceptButtonClick();
    });
    _controls.BTN_DONE->setOnClick([this]() {
        changeParty();
        _game.openInGame();
        if (!_context.exitScript.empty()) {
            _game.scriptRunner().run(_context.exitScript);
        }
    });
    _controls.BTN_BACK->setOnClick([this]() {
        _game.openInGame();
        if (!_context.exitScript.empty()) {
            _game.scriptRunner().run(_context.exitScript);
        }
    });
    _controls.BTN_NPC0->setOnClick([this]() {
        onNpcButtonClick(0);
    });
    _controls.BTN_NPC1->setOnClick([this]() {
        onNpcButtonClick(1);
    });
    _controls.BTN_NPC2->setOnClick([this]() {
        onNpcButtonClick(2);
    });
    _controls.BTN_NPC3->setOnClick([this]() {
        onNpcButtonClick(3);
    });
    _controls.BTN_NPC4->setOnClick([this]() {
        onNpcButtonClick(4);
    });
    _controls.BTN_NPC5->setOnClick([this]() {
        onNpcButtonClick(5);
    });
    _controls.BTN_NPC6->setOnClick([this]() {
        onNpcButtonClick(6);
    });
    _controls.BTN_NPC7->setOnClick([this]() {
        onNpcButtonClick(7);
    });
    _controls.BTN_NPC8->setOnClick([this]() {
        onNpcButtonClick(8);
    });

    if (_game.isTSL()) {
        _controls.BTN_NPC9->setOnClick([this]() {
            onNpcButtonClick(9);
        });
        _controls.BTN_NPC10->setOnClick([this]() {
            onNpcButtonClick(10);
        });
        _controls.BTN_NPC11->setOnClick([this]() {
            onNpcButtonClick(11);
        });
    }
}

void PartySelection::prepare(const PartySelectionContext &ctx) {
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
    Party &party = _game.party();
    std::vector<Label *> charLabels {
        _controls.LBL_CHAR0.get(),
        _controls.LBL_CHAR1.get(),
        _controls.LBL_CHAR2.get(),
        _controls.LBL_CHAR3.get(),
        _controls.LBL_CHAR4.get(),
        _controls.LBL_CHAR5.get(),
        _controls.LBL_CHAR6.get(),
        _controls.LBL_CHAR7.get(),
        _controls.LBL_CHAR8.get(),
    };
    std::vector<Label *> naLabels {
        _controls.LBL_NA0.get(),
        _controls.LBL_NA1.get(),
        _controls.LBL_NA2.get(),
        _controls.LBL_NA3.get(),
        _controls.LBL_NA4.get(),
        _controls.LBL_NA5.get(),
        _controls.LBL_NA6.get(),
        _controls.LBL_NA7.get(),
        _controls.LBL_NA8.get()};
    if (_game.isTSL()) {
        charLabels.push_back(_controls.LBL_CHAR9.get());
        charLabels.push_back(_controls.LBL_CHAR10.get());
        charLabels.push_back(_controls.LBL_CHAR11.get());
        naLabels.push_back(_controls.LBL_NA9.get());
        naLabels.push_back(_controls.LBL_NA10.get());
        naLabels.push_back(_controls.LBL_NA11.get());
    }

    for (int i = 0; i < kNpcCount; ++i) {
        ToggleButton &BTN_NPC = getNpcButton(i);
        Label &LBL_CHAR = *charLabels[i];
        Label &LBL_NA = *naLabels[i];

        if (party.isMemberAvailable(i)) {
            std::string blueprintResRef(party.getAvailableMember(i));
            std::shared_ptr<Gff> utc(_services.resource.gffs.get(blueprintResRef, ResType::Utc));
            std::shared_ptr<Texture> portrait;
            int portraitId = utc->getInt("PortraitId", 0);
            if (portraitId > 0) {
                portrait = _services.game.portraits.getTextureByIndex(portraitId);
            } else {
                int appearance = utc->getInt("Appearance_Type");
                portrait = _services.game.portraits.getTextureByAppearance(appearance);
            }
            BTN_NPC.setDisabled(false);
            LBL_CHAR.setBorderFill(std::move(portrait));
            LBL_NA.setVisible(false);
        } else {
            BTN_NPC.setDisabled(true);
            LBL_CHAR.setBorderFill(std::shared_ptr<Texture>(nullptr));
            LBL_NA.setVisible(true);
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
    std::vector<ToggleButton *> npcButtons {
        _controls.BTN_NPC0.get(),
        _controls.BTN_NPC1.get(),
        _controls.BTN_NPC2.get(),
        _controls.BTN_NPC3.get(),
        _controls.BTN_NPC4.get(),
        _controls.BTN_NPC5.get(),
        _controls.BTN_NPC6.get(),
        _controls.BTN_NPC7.get(),
        _controls.BTN_NPC8.get(),
        _controls.BTN_NPC9.get()};
    if (_game.isTSL()) {
        npcButtons.push_back(_controls.BTN_NPC10.get());
        npcButtons.push_back(_controls.BTN_NPC11.get());
    }
    return *npcButtons[npc];
}

void PartySelection::onAcceptButtonClick() {
    if (_selectedNpc == -1)
        return;

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
    _controls.LBL_COUNT->setTextMessage(std::to_string(_availableCount));
}

void PartySelection::refreshAcceptButton() {
    std::string text(_services.resource.strings.getText(_added[_selectedNpc] ? g_strRefRemove : g_strRefAdd));
    _controls.BTN_ACCEPT->setTextMessage(text);
}

void PartySelection::removeNpc(int npc) {
    ++_availableCount;
    _added[npc] = false;
    getNpcButton(npc).setUseBorderColorOverride(false);
    refreshAvailableCount();
}

void PartySelection::onNpcButtonClick(int npc) {
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
    std::shared_ptr<Area> area(_game.module()->area());
    area->unloadParty();

    Party &party = _game.party();
    party.clear();
    party.addMember(kNpcPlayer, party.player());

    std::shared_ptr<Creature> player(_game.party().player());

    for (int i = 0; i < kNpcCount; ++i) {
        if (!_added[i])
            continue;

        std::string blueprintResRef(party.getAvailableMember(i));

        std::shared_ptr<Creature> creature = _game.newCreature();
        creature->loadFromBlueprint(blueprintResRef);
        creature->setFaction(Faction::Friendly1);
        creature->setImmortal(true);
        party.addMember(i, creature);
    }

    area->reloadParty();
}

} // namespace game

} // namespace reone
