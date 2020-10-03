/*
 * Copyright © 2020 Vsevolod Kremianskii
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
#include "../../core/debug.h"
#include "../../gui/control/listbox.h"
#include "../../resource/resources.h"

#include "../util.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const float kKotorModelScale = 1.3f;
static const float kKotorModelOffsetY = 1.25f;

MainMenu::MainMenu(const Options &opts) : GUI(opts.graphics) {
    _resolutionX = 800;
    _resolutionY = 600;
}

void MainMenu::load(GameVersion version) {
    string resRef;
    BackgroundType background;

    switch (version) {
        case GameVersion::TheSithLords:
            resRef = "mainmenu8x6_p";
            background = BackgroundType::None;
            break;
        default:
            resRef = "mainmenu16x12";
            background = BackgroundType::Menu;
            break;
    }

    GUI::load(resRef, background);
    hideControl("BTN_MOREGAMES");
    hideControl("BTN_TSLRCM");
    hideControl("LB_MODULES");
    hideControl("LBL_NEWCONTENT");
    hideControl("LBL_BW");
    hideControl("LBL_LUCAS");

    if (getDebugLevel() == 0) {
        hideControl("BTN_WARP");
    }
    _version = version;
    configureButtons();

    if (_version == GameVersion::KotOR) {
        unique_ptr<SceneGraph> sceneGraph(new SceneGraph(_gfxOpts));

        Control &control = getControl("LBL_3DVIEW");
        const Control::Extent &extent = control.extent();

        shared_ptr<ModelSceneNode> model(new ModelSceneNode(sceneGraph.get(), Resources.findModel("mainmenu")));
        model->setDefaultAnimation("default");
        model->playDefaultAnimation();
        model->setLightingEnabled(true);

        float aspect = extent.width / static_cast<float>(extent.height);

        glm::mat4 projection(glm::ortho(
            -aspect * kKotorModelScale,
            aspect * kKotorModelScale,
            -kKotorModelScale + kKotorModelOffsetY,
            kKotorModelScale + kKotorModelOffsetY,
            0.1f,
            2.0f));

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

        shared_ptr<CameraSceneNode> camera(new CameraSceneNode(sceneGraph.get(), projection));
        camera->setLocalTransform(transform);

        sceneGraph->addRoot(model);
        sceneGraph->setAmbientLightColor(glm::vec3(0.1f));
        sceneGraph->setActiveCamera(camera);

        unique_ptr<Control::Scene3D> scene(new Control::Scene3D());
        scene->model = model;
        scene->sceneGraph = move(sceneGraph);

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

void MainMenu::onClick(const string &control) {
    if (control == "BTN_NEWGAME") {
        if (_onNewGame) {
            _onNewGame();
        }
    } else if (control == "BTN_EXIT") {
        if (_onExit) _onExit();
    } else if (control == "BTN_WARP") {
        startModuleSelection();
    }
}

void MainMenu::startModuleSelection() {
    hideControl("BTN_EXIT");
    hideControl("BTN_LOADGAME");
    hideControl("BTN_MOVIES");
    hideControl("BTN_MUSIC");
    hideControl("BTN_NEWGAME");
    hideControl("BTN_OPTIONS");
    hideControl("BTN_WARP");
    showControl("LB_MODULES");
    hideControl("LBL_3DVIEW");
    hideControl("LBL_GAMELOGO");
    hideControl("LBL_MENUBG");

    ListBox &modules = static_cast<ListBox &>(getControl("LB_MODULES"));
    modules.setOnItemClicked([this](const string &ctrl, const string &item) {
        if (_onModuleSelected) {
            _onModuleSelected(item);
        }
    });
    for (auto &module : Resources.moduleNames()) {
        modules.add({ module, module });
    }
}

void MainMenu::setOnNewGame(const function<void()> &fn) {
    _onNewGame = fn;
}

void MainMenu::setOnExit(const function<void()> &fn) {
    _onExit = fn;
}

void MainMenu::setOnModuleSelected(const function<void(const string &)> &fn) {
    _onModuleSelected = fn;
}

} // namespace game

} // namespace reone
