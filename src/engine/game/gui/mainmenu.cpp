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

#include "mainmenu.h"

#include "glm/glm.hpp"

#include "../../audio/player.h"
#include "../../common/log.h"
#include "../../gui/control/listbox.h"
#include "../../gui/scenebuilder.h"
#include "../../graphics/model/models.h"
#include "../../resource/resources.h"
#include "../../scene/types.h"

#include "../game.h"
#include "../gameidutil.h"

#include "colorutil.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kKotorModelSize = 1.4f;

static const char kBlueprintResRefCarth[] = "p_carth";
static const char kBlueprintResRefBastila[] = "p_bastilla";
static const char kBlueprintResRefAtton[] = "p_atton";
static const char kBlueprintResRefKreia[] = "p_kreia";

MainMenu::MainMenu(Game *game) :
    GameGUI(game->gameId(), game->options().graphics),
    _game(game) {

    if (isTSL(game->gameId())) {
        _resRef = "mainmenu8x6_p";
    } else {
        _resRef = "mainmenu16x12";
        loadBackground(BackgroundType::Menu);
    }

    _resolutionX = 800;
    _resolutionY = 600;
}

void MainMenu::load() {
    GUI::load();

    hideControl("BTN_MOREGAMES");
    hideControl("BTN_TSLRCM");
    hideControl("LB_MODULES");
    hideControl("LBL_NEWCONTENT");
    hideControl("LBL_BW");
    hideControl("LBL_LUCAS");

    setControlDisabled("BTN_MOVIES", true);
    setControlDisabled("BTN_OPTIONS", true);

    // Hide warp button in developer mode
    if (!_game->options().developer) {
        hideControl("BTN_WARP");
    }

    setup3DView();
    configureButtons();
}

void MainMenu::configureButtons() {
    setButtonColors("BTN_EXIT");
    setButtonColors("BTN_LOADGAME");
    setButtonColors("BTN_MOVIES");
    setButtonColors("BTN_NEWGAME");
    setButtonColors("BTN_OPTIONS");

    if (isTSL(_gameId)) {
        setButtonColors("BTN_MUSIC");
    }
}

void MainMenu::setButtonColors(const string &tag) {
    Control &control = getControl(tag);
    control.setTextColor(getBaseColor(_gameId));
    control.setHilightColor(getHilightColor(_gameId));
}

void MainMenu::setup3DView() {
    if (_gameId != GameID::KotOR) return;

    Control &control = getControl("LBL_3DVIEW");
    const Control::Extent &extent = control.extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    unique_ptr<SceneGraph> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&MainMenu::getKotorModel, this, _1))
        .modelScale(kKotorModelSize)
        .cameraFromModelNode("camerahook")
        .ambientLightColor(glm::vec3(0.1f))
        .build());

    control.setScene(move(scene));
}

shared_ptr<ModelSceneNode> MainMenu::getKotorModel(SceneGraph &sceneGraph) {
    auto model = make_shared<ModelSceneNode>(ModelUsage::GUI, Models::instance().get("mainmenu"), &sceneGraph);
    model->animator().playAnimation("default", AnimationProperties::fromFlags(AnimationFlags::loop));

    return move(model);
}

void MainMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_NEWGAME") {
        _game->startCharacterGeneration();
    } else if (control == "BTN_LOADGAME") {
        _game->openSaveLoad(SaveLoad::Mode::LoadFromMainMenu);
    } else if (control == "BTN_EXIT") {
        _game->quit();
    } else if (control == "BTN_WARP") {
        startModuleSelection();
    }
}

void MainMenu::startModuleSelection() {
    showControl("LB_MODULES");
    hideControl("BTN_EXIT");
    hideControl("BTN_LOADGAME");
    hideControl("BTN_MOVIES");
    hideControl("BTN_MUSIC");
    hideControl("BTN_NEWGAME");
    hideControl("BTN_OPTIONS");
    hideControl("BTN_WARP");
    hideControl("LBL_3DVIEW");
    hideControl("LBL_GAMELOGO");
    hideControl("LBL_MENUBG");

    loadModuleNames();
}

void MainMenu::loadModuleNames() {
    auto &modules = getControl<ListBox>("LB_MODULES");
    for (auto &module : _game->moduleNames()) {
        ListBox::Item item;
        item.tag = module;
        item.text = module;
        modules.addItem(move(item));
    }
}

void MainMenu::onListBoxItemClick(const string &control, const string &item) {
    if (control == "LB_MODULES") {
        onModuleSelected(item);
    }
}

void MainMenu::onModuleSelected(const string &name) {
    string member1Blueprint;
    string member2Blueprint;
    string member3Blueprint;

    if (isTSL(_gameId)) {
        member1Blueprint = kBlueprintResRefAtton;
        member2Blueprint = kBlueprintResRefKreia;
    } else {
        member1Blueprint = kBlueprintResRefCarth;
        member2Blueprint = kBlueprintResRefBastila;
    }
    shared_ptr<TwoDA> defaultParty(Resources::instance().get2DA("defaultparty"));
    if (defaultParty) {
        for (int row = 0; row < defaultParty->getRowCount(); ++row) {
            if (defaultParty->getBool(row, "tsl") == isTSL(_gameId)) {
                member1Blueprint = defaultParty->getString(row, "partymember0");
                member2Blueprint = defaultParty->getString(row, "partymember1");
                member3Blueprint = defaultParty->getString(row, "partymember2");
                break;
            }
        }
    }

    Party &party = _game->party();
    if (!member1Blueprint.empty()) {
        shared_ptr<Creature> player(_game->objectFactory().newCreature());
        player->loadFromBlueprint(member1Blueprint);
        player->setTag(kObjectTagPlayer);
        player->setImmortal(true);
        party.addMember(kNpcPlayer, player);
        party.setPlayer(player);
    }
    if (!member2Blueprint.empty()) {
        shared_ptr<Creature> companion(_game->objectFactory().newCreature());
        companion->loadFromBlueprint(member2Blueprint);
        companion->setImmortal(true);
        party.addMember(0, companion);
    }
    if (!member3Blueprint.empty()) {
        shared_ptr<Creature> companion(_game->objectFactory().newCreature());
        companion->loadFromBlueprint(member3Blueprint);
        companion->setImmortal(true);
        party.addMember(1, companion);
    }

    _game->loadModule(name);
}

} // namespace game

} // namespace reone
