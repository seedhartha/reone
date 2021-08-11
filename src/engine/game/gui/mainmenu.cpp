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
#include "../../gui/scenebuilder.h"
#include "../../graphics/model/models.h"
#include "../../resource/resources.h"
#include "../../scene/types.h"

#include "../game.h"

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
    if (game->isTSL()) {
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

    if (_game->isTSL()) {
        _binding.btnMoreGames->setVisible(false);
        _binding.btnTslrcm->setVisible(false);
    }

    // Hide warp button in developer mode
    if (!_game->options().developer) {
        _binding.btnWarp->setVisible(false);
    }

    _binding.btnNewGame->setOnClick([this]() {
        _game->startCharacterGeneration();
    });
    _binding.btnLoadGame->setOnClick([this]() {
        _game->openSaveLoad(SaveLoad::Mode::LoadFromMainMenu);
    });
    _binding.btnExit->setOnClick([this]() {
        _game->quit();
    });
    _binding.btnWarp->setOnClick([this]() {
        startModuleSelection();
    });
    _binding.lbModules->setOnItemClick([this](const string &item) {
        onModuleSelected(item);
    });

    setup3DView();
    configureButtons();
}

void MainMenu::bindControls() {
    _binding.lbModules = getControl<ListBox>("LB_MODULES");
    _binding.lbl3dView = getControl<Label>("LBL_3DVIEW");
    _binding.lblGameLogo = getControl<Label>("LBL_GAMELOGO");
    _binding.lblBw = getControl<Label>("LBL_BW");
    _binding.lblLucas = getControl<Label>("LBL_LUCAS");
    _binding.btnLoadGame = getControl<Button>("BTN_LOADGAME");
    _binding.btnNewGame = getControl<Button>("BTN_NEWGAME");
    _binding.btnMovies = getControl<Button>("BTN_MOVIES");
    _binding.btnOptions = getControl<Button>("BTN_OPTIONS");
    _binding.lblNewContent = getControl<Label>("LBL_NEWCONTENT");
    _binding.btnExit = getControl<Button>("BTN_EXIT");
    _binding.btnWarp = getControl<Button>("BTN_WARP");

    if (_game->isTSL()) {
        _binding.btnMusic = getControl<Button>("BTN_MUSIC");
        _binding.btnMoreGames = getControl<Button>("BTN_MOREGAMES");
        _binding.btnTslrcm = getControl<Button>("BTN_TSLRCM");
    } else {
        _binding.lblMenuBg = getControl<Label>("LBL_MENUBG");
    }
}

void MainMenu::configureButtons() {
    setButtonColors(*_binding.btnExit);
    setButtonColors(*_binding.btnLoadGame);
    setButtonColors(*_binding.btnMovies);
    setButtonColors(*_binding.btnNewGame);
    setButtonColors(*_binding.btnOptions);

    if (_game->isTSL()) {
        setButtonColors(*_binding.btnMusic);
    }
}

void MainMenu::setButtonColors(Control &control) {
    control.setTextColor(_game->getGUIColorBase());
    control.setHilightColor(_game->getGUIColorHilight());
}

void MainMenu::setup3DView() {
    if (_game->id() != GameID::KotOR) return;

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

void MainMenu::startModuleSelection() {
    loadModuleNames();

    _binding.lbModules->setVisible(true);
    _binding.btnExit->setVisible(false);
    _binding.btnLoadGame->setVisible(false);
    _binding.btnMovies->setVisible(false);
    _binding.btnNewGame->setVisible(false);
    _binding.btnOptions->setVisible(false);
    _binding.btnWarp->setVisible(false);
    _binding.lbl3dView->setVisible(false);
    _binding.lblGameLogo->setVisible(false);

    if (_game->isTSL()) {
        _binding.btnMusic->setVisible(false);
    } else {
        _binding.lblMenuBg->setVisible(false);
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

void MainMenu::onModuleSelected(const string &name) {
    string member1Blueprint;
    string member2Blueprint;
    string member3Blueprint;

    if (_game->isTSL()) {
        member1Blueprint = kBlueprintResRefAtton;
        member2Blueprint = kBlueprintResRefKreia;
    } else {
        member1Blueprint = kBlueprintResRefCarth;
        member2Blueprint = kBlueprintResRefBastila;
    }
    shared_ptr<TwoDA> defaultParty(_game->services().resource().resources().get2DA("defaultparty"));
    if (defaultParty) {
        for (int row = 0; row < defaultParty->getRowCount(); ++row) {
            if (defaultParty->getBool(row, "tsl") == _game->isTSL()) {
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
