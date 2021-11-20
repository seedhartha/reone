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

#include "../../../audio/player.h"
#include "../../../common/logutil.h"
#include "../../../graphics/model/models.h"
#include "../../../gui/sceneinitializer.h"
#include "../../../scene/graphs.h"
#include "../../../scene/types.h"

#include "../../core/object/factory.h"
#include "../../core/party.h"
#include "../../core/services.h"

#include "../kotor.h"
#include "../types.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kKotorModelSize = 1.4f;

MainMenu::MainMenu(KotOR &game, Services &services) :
    GameGUI(game, services) {
    if (game.isTSL()) {
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

    if (_binding.btnMoreGames) {
        _binding.btnMoreGames->setVisible(false);
    }
    if (_binding.btnTslrcm) {
        _binding.btnTslrcm->setVisible(false);
    }

    // Hide warp button in developer mode
    if (!_game.options().developer) {
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

    if (_game.isTSL()) {
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

    if (_game.isTSL()) {
        setButtonColors(*_binding.btnMusic);
    }
}

void MainMenu::setButtonColors(Control &control) {
    control.setTextColor(_game.getGUIColorBase());
    control.setHilightColor(_game.getGUIColorHilight());
}

void MainMenu::setup3DView() {
    if (_game.isTSL()) {
        return;
    }

    auto &sceneGraph = _services.sceneGraphs.get(kSceneMainMenu);
    const Control::Extent &extent = _binding.lbl3dView->extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&MainMenu::getKotorModel, this, _1))
        .modelScale(kKotorModelSize)
        .cameraFromModelNode("camerahook")
        .lightingRefFromModelNode("rootdummy")
        .invoke();

    _binding.lbl3dView->setSceneName(kSceneMainMenu);
}

shared_ptr<ModelSceneNode> MainMenu::getKotorModel(SceneGraph &sceneGraph) {
    auto model = sceneGraph.newModel(_services.models.get("mainmenu"), ModelUsage::GUI);
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
        _binding.lbModules->addItem(move(item));
    }
}

void MainMenu::onModuleSelected(const string &name) {
    _game.loadModule(name);
}

} // namespace game

} // namespace reone
