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
#include "../../audio/util.h"
#include "../../common/log.h"
#include "../../gui/control/listbox.h"
#include "../../gui/scenebuilder.h"
#include "../../render/models.h"
#include "../../resource/resources.h"

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

static constexpr float kKotorModelSize = 1.3f;
static constexpr float kKotorModelOffsetY = 1.25f;

static constexpr char kBlueprintResRefCarth[] = "p_carth";
static constexpr char kBlueprintResRefBastila[] = "p_bastilla";
static constexpr char kBlueprintResRefAtton[] = "p_atton";
static constexpr char kBlueprintResRefKreia[] = "p_kreia";

static bool g_warpEnabled = true;

MainMenu::MainMenu(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    switch (game->version()) {
        case GameVersion::TheSithLords:
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

    if (!g_warpEnabled) {
        hideControl("BTN_WARP");
    }
    configureButtons();

    if (_version == GameVersion::KotOR) {
        Control &control = getControl("LBL_3DVIEW");
        const Control::Extent &extent = control.extent();
        float aspect = extent.width / static_cast<float>(extent.height);

        glm::mat4 cameraTransform(1.0f);
        cameraTransform = glm::rotate(cameraTransform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

        unique_ptr<Control::Scene3D> scene(SceneBuilder(_gfxOpts)
            .aspect(aspect)
            .depth(0.1f, 2.0f)
            .modelSupplier(bind(&MainMenu::getKotorModel, this, _1))
            .modelScale(kKotorModelSize)
            .modelOffset(glm::vec2(0.0f, kKotorModelOffsetY))
            .cameraTransform(cameraTransform)
            .ambientLightColor(glm::vec3(0.1f))
            .build());

        control.setScene3D(move(scene));
    }
}

void MainMenu::configureButtons() {
    setButtonColors("BTN_EXIT");
    setButtonColors("BTN_LOADGAME");
    setButtonColors("BTN_MOVIES");
    setButtonColors("BTN_NEWGAME");
    setButtonColors("BTN_OPTIONS");

    if (_version == GameVersion::TheSithLords) {
        setButtonColors("BTN_MUSIC");
    }
}

void MainMenu::setButtonColors(const string &tag) {
    Control &control = getControl(tag);

    Control::Text text(control.text());
    text.color = getBaseColor(_version);
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = getHilightColor(_version);
    control.setHilight(move(hilight));
}

shared_ptr<ModelSceneNode> MainMenu::getKotorModel(SceneGraph &sceneGraph) {
    shared_ptr<ModelSceneNode> model(new ModelSceneNode(&sceneGraph, Models::instance().get("mainmenu")));
    model->setDefaultAnimation("default");
    model->playDefaultAnimation();
    model->setLightingEnabled(true);

    return move(model);
}

void MainMenu::onClick(const string &control) {
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

    ListBox &modules = static_cast<ListBox &>(getControl("LB_MODULES"));
    for (auto &module : Resources::instance().moduleNames()) {
        modules.addItem({ module, module });
    }
}

void MainMenu::onListBoxItemClick(const string &control, const string &item) {
    if (control != "LB_MODULES") return;

    onModuleSelected(item);
}

void MainMenu::onModuleSelected(const string &name) {
    shared_ptr<CreatureBlueprint> playerBlueprint;
    shared_ptr<CreatureBlueprint> companionBlueprint;

    switch (_version) {
        case GameVersion::TheSithLords:
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

    switch (_version) {
        case GameVersion::TheSithLords:
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
