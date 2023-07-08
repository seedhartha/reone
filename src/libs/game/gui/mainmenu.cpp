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

#include "reone/game/gui/mainmenu.h"

#include "reone/audio/player.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/models.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/types.h"
#include "reone/system/logutil.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/factory.h"
#include "reone/game/party.h"
#include "reone/game/types.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kKotorModelSize = 1.4f;

MainMenu::MainMenu(Game &game, ServicesView &services) :
    GameGUI(game, services) {

    if (_game.isTSL()) {
        _resRef = "mainmenu8x6_p";
        _musicResRef = "mus_sion";
    } else {
        _resRef = "mainmenu16x12";
        _musicResRef = "mus_theme_cult";
    }
}

void MainMenu::preload(IGUI &gui) {
    GameGUI::preload(gui);
    gui.setResolution(800, 600);
}

void MainMenu::onGUILoaded() {
    if (!_game.isTSL()) {
        loadBackground(BackgroundType::Menu);
    }

    bindControls();

    _binding.lbModules->setVisible(false);
    _binding.lblNewContent->setVisible(false);
    _binding.lblBw->setVisible(false);
    _binding.lblLucas->setVisible(false);
    _binding.btnMovies->setDisabled(true);
    _binding.btnOptions->setDisabled(true);

    if (_binding.btnMoreGames) {
        _binding.btnMoreGames->setVisible(false);
    }
    if (_binding.btnTslrcm) {
        _binding.btnTslrcm->setVisible(false);
    }

    // Hide warp button in developer mode
    if (!_game.options().game.developer) {
        _binding.btnWarp->setVisible(false);
    }

    _binding.btnNewGame->setOnClick([this]() {
        _game.startCharacterGeneration();
    });
    _binding.btnLoadGame->setOnClick([this]() {
        _game.openSaveLoad(SaveLoadMode::LoadFromMainMenu);
    });
    _binding.btnExit->setOnClick([this]() {
        _game.quit();
    });
    _binding.btnWarp->setOnClick([this]() {
        startModuleSelection();
    });
    _binding.lbModules->setOnItemClick([this](const std::string &item) {
        onModuleSelected(item);
    });

    setup3DView();
    configureButtons();
}

void MainMenu::bindControls() {
    _binding.lbModules = findControl<ListBox>("LB_MODULES");
    _binding.lbl3dView = findControl<Label>("LBL_3DVIEW");
    _binding.lblGameLogo = findControl<Label>("LBL_GAMELOGO");
    _binding.lblBw = findControl<Label>("LBL_BW");
    _binding.lblLucas = findControl<Label>("LBL_LUCAS");
    _binding.btnLoadGame = findControl<Button>("BTN_LOADGAME");
    _binding.btnNewGame = findControl<Button>("BTN_NEWGAME");
    _binding.btnMovies = findControl<Button>("BTN_MOVIES");
    _binding.btnOptions = findControl<Button>("BTN_OPTIONS");
    _binding.lblNewContent = findControl<Label>("LBL_NEWCONTENT");
    _binding.btnExit = findControl<Button>("BTN_EXIT");
    _binding.btnWarp = findControl<Button>("BTN_WARP");

    if (_game.isTSL()) {
        _binding.btnMusic = findControl<Button>("BTN_MUSIC");
        _binding.btnMoreGames = findControl<Button>("BTN_MOREGAMES");
        _binding.btnTslrcm = findControl<Button>("BTN_TSLRCM");
    } else {
        _binding.lblMenuBg = findControl<Label>("LBL_MENUBG");
    }
}

void MainMenu::configureButtons() {
    setButtonColors(*_binding.btnExit);
    setButtonColors(*_binding.btnLoadGame);
    setButtonColors(*_binding.btnMovies);
    setButtonColors(*_binding.btnNewGame);
    setButtonColors(*_binding.btnOptions);

    if (_game.isTSL()) {
        setButtonColors(*_binding.btnMusic);
    }
}

void MainMenu::setButtonColors(Control &control) {
    control.setTextColor(_baseColor);
    control.setHilightColor(_hilightColor);
}

void MainMenu::setup3DView() {
    if (_game.isTSL()) {
        return;
    }

    auto &sceneGraph = _services.scene.graphs.get(kSceneMainMenu);
    const Control::Extent &extent = _binding.lbl3dView->extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(kDefaultClipPlaneNear, 10.0f)
        .modelSupplier(bind(&MainMenu::getKotorModel, this, std::placeholders::_1))
        .modelScale(kKotorModelSize)
        .cameraFromModelNode("camerahook")
        .invoke();

    _binding.lbl3dView->setSceneName(kSceneMainMenu);
}

std::shared_ptr<ModelSceneNode> MainMenu::getKotorModel(ISceneGraph &sceneGraph) {
    auto model = _services.graphics.models.get("mainmenu");
    if (!model) {
        return nullptr;
    }
    return sceneGraph.newModel(*model, ModelUsage::GUI);
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

    if (_game.isTSL()) {
        _binding.btnMusic->setVisible(false);
    } else {
        _binding.lblMenuBg->setVisible(false);
    }
}

void MainMenu::loadModuleNames() {
    for (auto &module : _game.moduleNames()) {
        ListBox::Item item;
        item.tag = module;
        item.text = module;
        _binding.lbModules->addItem(std::move(item));
    }
}

void MainMenu::onModuleSelected(const std::string &name) {
    _game.loadModule(name);
}

} // namespace game

} // namespace reone
