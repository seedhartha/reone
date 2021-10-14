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
#include "../../../gui/scenebuilder.h"
#include "../../../scene/types.h"

#include "../../core/object/factory.h"
#include "../../core/party.h"

#include "../kotor.h"

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

MainMenu::MainMenu(
    KotOR *game,
    ActionFactory &actionFactory,
    Classes &classes,
    Combat &combat,
    Feats &feats,
    FootstepSounds &footstepSounds,
    GUISounds &guiSounds,
    ObjectFactory &objectFactory,
    Party &party,
    Portraits &portraits,
    Reputes &reputes,
    ScriptRunner &scriptRunner,
    SoundSets &soundSets,
    Surfaces &surfaces,
    audio::AudioFiles &audioFiles,
    audio::AudioPlayer &audioPlayer,
    graphics::Context &context,
    graphics::Features &features,
    graphics::Fonts &fonts,
    graphics::Lips &lips,
    graphics::Materials &materials,
    graphics::Meshes &meshes,
    graphics::Models &models,
    graphics::PBRIBL &pbrIbl,
    graphics::Shaders &shaders,
    graphics::Textures &textures,
    graphics::Walkmeshes &walkmeshes,
    graphics::Window &window,
    resource::Resources &resources,
    resource::Strings &strings) :
    GameGUI(
        game,
        actionFactory,
        classes,
        combat,
        feats,
        footstepSounds,
        guiSounds,
        objectFactory,
        party,
        portraits,
        reputes,
        scriptRunner,
        soundSets,
        surfaces,
        audioFiles,
        audioPlayer,
        context,
        features,
        fonts,
        lips,
        materials,
        meshes,
        models,
        pbrIbl,
        shaders,
        textures,
        walkmeshes,
        window,
        resources,
        strings) {
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

    if (_binding.btnMoreGames) {
        _binding.btnMoreGames->setVisible(false);
    }
    if (_binding.btnTslrcm) {
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
        _game->openSaveLoad(SaveLoadMode::LoadFromMainMenu);
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
    if (_game->isTSL())
        return;

    const Control::Extent &extent = _binding.lbl3dView->extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    unique_ptr<SceneGraph> scene(SceneBuilder(
                                     _options,
                                     _context,
                                     _features,
                                     _materials,
                                     _meshes,
                                     _pbrIbl,
                                     _shaders,
                                     _textures)
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
    auto model = sceneGraph.newModel(_models.get("mainmenu"), ModelUsage::GUI);
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
    _game->loadModule(name);
}

} // namespace game

} // namespace reone
