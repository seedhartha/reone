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
#include "../../render/model/models.h"
#include "../../resource/resources.h"
#include "../../scene/types.h"

#include "../blueprint/blueprints.h"
#include "../game.h"

#include "colorutil.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
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

    switch (game->gameId()) {
        case GameID::TSL:
            _resRef = "mainmenu8x6_p";
            break;
        default:
            _resRef = "mainmenu16x12";
            _backgroundType = BackgroundType::Menu;
            break;
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

    if (_gameId == GameID::TSL) {
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

    unique_ptr<Control::Scene3D> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&MainMenu::getKotorModel, this, _1))
        .modelScale(kKotorModelSize)
        .cameraFromModelNode("camerahook")
        .ambientLightColor(glm::vec3(0.1f))
        .build());

    control.setScene3D(move(scene));
}

shared_ptr<ModelSceneNode> MainMenu::getKotorModel(SceneGraph &sceneGraph) {
    auto model = make_shared<ModelSceneNode>(&sceneGraph, Models::instance().get("mainmenu"));
    model->setLightingEnabled(true);
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
    for (auto &module : Resources::instance().moduleNames()) {
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
    shared_ptr<CreatureBlueprint> playerBlueprint;
    shared_ptr<CreatureBlueprint> companionBlueprint;

    switch (_gameId) {
        case GameID::TSL:
            playerBlueprint = Blueprints::instance().getCreature(kBlueprintResRefAtton);
            companionBlueprint = Blueprints::instance().getCreature(kBlueprintResRefKreia);
            break;
        default:
            playerBlueprint = Blueprints::instance().getCreature(kBlueprintResRefCarth);
            companionBlueprint = Blueprints::instance().getCreature(kBlueprintResRefBastila);
            break;
    }

    Party &party = _game->party();

    shared_ptr<Creature> player(_game->objectFactory().newCreature());
    player->load(playerBlueprint);
    player->setTag(kObjectTagPlayer);
    player->setImmortal(true);
    party.addMember(kNpcPlayer, player);
    party.setPlayer(player);

    shared_ptr<Creature> companion(_game->objectFactory().newCreature());
    companion->load(companionBlueprint);
    companion->setImmortal(true);
    party.addMember(0, companion);

    switch (_gameId) {
        case GameID::TSL:
            player->equip("w_blaste_01");
            companion->equip("w_melee_06");
            break;
        default:
            companion->equip("g_w_dblsbr004");
            break;
    }

    _game->loadModule(name);
}

} // namespace game

} // namespace reone
