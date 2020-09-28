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

#include "../../audio/player.h"
#include "../../core/debug.h"
#include "../../gui/control/listbox.h"
#include "../../resources/resources.h"

#include "../util.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

MainMenu::MainMenu(const Options &opts) : GUI(opts.graphics), _opts(opts) {
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
        Control &control = getControl("LBL_3DVIEW");
        const Control::Extent &extent = control.extent();
        float scale = extent.height / 2.8f;

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, 0.9286f * extent.height, 0.0f));
        transform = glm::rotate(transform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(scale));

        Control::Scene3D scene;
        scene.model = make_shared<ModelSceneNode>(nullptr, Resources.findModel("mainmenu"));
        scene.transform = move(transform);

        control.setScene3D(scene);

        scene.model->setDefaultAnimation("default");
        scene.model->playDefaultAnimation();
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
