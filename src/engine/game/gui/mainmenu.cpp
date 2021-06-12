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

MainMenu::MainMenu(Game *game) : GameGUI(game) {
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
    bindControls();

    _binding.lbModules->setVisible(false);
    _binding.lblNewContent->setVisible(false);
    _binding.lblBw->setVisible(false);
    _binding.lblLucas->setVisible(false);
    _binding.btnMovies->setDisabled(true);
    _binding.btnOptions->setDisabled(true);

    // Hide warp button in developer mode
    if (!_game->options().developer) {
        _binding.btnWarp->setVisible(false);
    }

    setup3DView();
    configureButtons();
}

void MainMenu::bindControls() {
    _binding.lbModules = getControlPtr<ListBox>("LB_MODULES");
    _binding.lbl3dView = getControlPtr<Label>("LBL_3DVIEW");
    _binding.lblGameLogo = getControlPtr<Label>("LBL_GAMELOGO");
    _binding.lblBw = getControlPtr<Label>("LBL_BW");
    _binding.lblLucas = getControlPtr<Label>("LBL_LUCAS");
    _binding.lblMenuBg = getControlPtr<Label>("LBL_MENUBG");
    _binding.btnLoadGame = getControlPtr<Button>("BTN_LOADGAME");
    _binding.btnNewGame = getControlPtr<Button>("BTN_NEWGAME");
    _binding.btnMovies = getControlPtr<Button>("BTN_MOVIES");
    _binding.btnOptions = getControlPtr<Button>("BTN_OPTIONS");
    _binding.lblNewContent = getControlPtr<Label>("LBL_NEWCONTENT");
    _binding.btnExit = getControlPtr<Button>("BTN_EXIT");
    _binding.btnWarp = getControlPtr<Button>("BTN_WARP");

    if (isTSL(_game->gameId())) {
        _binding.btnMusic = getControlPtr<Button>("BTN_MUSIC");
    }
}

void MainMenu::configureButtons() {
    setButtonColors(*_binding.btnExit);
    setButtonColors(*_binding.btnLoadGame);
    setButtonColors(*_binding.btnMovies);
    setButtonColors(*_binding.btnNewGame);
    setButtonColors(*_binding.btnOptions);

    if (isTSL(_game->gameId())) {
        setButtonColors(*_binding.btnMusic);
    }
}

void MainMenu::setButtonColors(Control &control) {
    control.setTextColor(getBaseColor(_game->gameId()));
    control.setHilightColor(getHilightColor(_game->gameId()));
}

void MainMenu::setup3DView() {
    if (_game->gameId() != GameID::KotOR) return;

    const Control::Extent &extent = _binding.lbl3dView->extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    unique_ptr<SceneGraph> scene(SceneBuilder(_options, _game->services().graphics())
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&MainMenu::getKotorModel, this, _1))
        .modelScale(kKotorModelSize)
        .cameraFromModelNode("camerahook")
        .lightingRefFromModelNode("rootdummy")
        .build());

    _binding.lbl3dView->setScene(move(scene));
}

shared_ptr<ModelSceneNode> MainMenu::getKotorModel(SceneGraph &sceneGraph) {
    auto model = make_shared<ModelSceneNode>(_game->services().graphics().models().get("mainmenu"), ModelUsage::GUI, &sceneGraph);
    model->playAnimation("default", AnimationProperties::fromFlags(AnimationFlags::loop));
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
    _binding.lbModules->setVisible(true);
    _binding.btnExit->setVisible(false);
    _binding.btnLoadGame->setVisible(false);
    _binding.btnMovies->setVisible(false);
    _binding.btnNewGame->setVisible(false);
    _binding.btnOptions->setVisible(false);
    _binding.btnWarp->setVisible(false);
    _binding.lbl3dView->setVisible(false);
    _binding.lblGameLogo->setVisible(false);
    _binding.lblMenuBg->setVisible(false);

    loadModuleNames();

    if (isTSL(_game->gameId())) {
        _binding.btnMusic->setVisible(false);
    }
}

void MainMenu::loadModuleNames() {
    for (auto &module : _game->moduleNames()) {
        ListBox::Item item;
        item.tag = module;
        item.text = module;
        _binding.lbModules->addItem(move(item));
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

    if (isTSL(_game->gameId())) {
        member1Blueprint = kBlueprintResRefAtton;
        member2Blueprint = kBlueprintResRefKreia;
    } else {
        member1Blueprint = kBlueprintResRefCarth;
        member2Blueprint = kBlueprintResRefBastila;
    }
    shared_ptr<TwoDA> defaultParty(_game->services().resource().resources().get2DA("defaultparty"));
    if (defaultParty) {
        for (int row = 0; row < defaultParty->getRowCount(); ++row) {
            if (defaultParty->getBool(row, "tsl") == isTSL(_game->gameId())) {
                member1Blueprint = defaultParty->getString(row, "partymember0");
                member2Blueprint = defaultParty->getString(row, "partymember1");
                member3Blueprint = defaultParty->getString(row, "partymember2");
                break;
            }
        }
    }

    Party &party = _game->services().party();
    if (!member1Blueprint.empty()) {
        shared_ptr<Creature> player(_game->services().objectFactory().newCreature());
        player->loadFromBlueprint(member1Blueprint);
        player->setTag(kObjectTagPlayer);
        player->setImmortal(true);
        party.addMember(kNpcPlayer, player);
        party.setPlayer(player);
    }
    if (!member2Blueprint.empty()) {
        shared_ptr<Creature> companion(_game->services().objectFactory().newCreature());
        companion->loadFromBlueprint(member2Blueprint);
        companion->setImmortal(true);
        party.addMember(0, companion);
    }
    if (!member3Blueprint.empty()) {
        shared_ptr<Creature> companion(_game->services().objectFactory().newCreature());
        companion->loadFromBlueprint(member3Blueprint);
        companion->setImmortal(true);
        party.addMember(1, companion);
    }

    _game->loadModule(name);
}

} // namespace game

} // namespace reone
