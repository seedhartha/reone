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

#include "../../graphics/models.h"
#include "../../graphics/services.h"
#include "../../graphics/types.h"
#include "../../gui/control/label.h"
#include "../../gui/control/listbox.h"
#include "../../gui/sceneinitializer.h"
#include "../../scene/graphs.h"
#include "../../scene/node/model.h"
#include "../../scene/services.h"

#include "../gameinterface.h"
#include "../options.h"
#include "../types.h"


using namespace std;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::scene;

namespace reone {

namespace game {

void MainMenu::init() {
    load("mainmenu16x12");
    bindControls();
    init3dView();
    initModules();

    _lbModules->setEnabled(false);

    disableControl("LBL_BW");
    disableControl("LBL_LUCAS");
    disableControl("LBL_NEWCONTENT");

    if (!_gameOpt.developer) {
        disableControl("BTN_WARP");
    }
}

void MainMenu::bindControls() {
    _lbl3dView = findControl<Label>("LBL_3DVIEW");
    _lbModules = findControl<ListBox>("LB_MODULES");
}

void MainMenu::init3dView() {
    auto &scene = _sceneSvc.graphs.get(kSceneMainMenu);
    float aspect = _lbl3dView->extent()[2] / static_cast<float>(_lbl3dView->extent()[3]);

    auto model = _graphicsSvc.models.get("mainmenu");
    auto modelSceneNode = shared_ptr<ModelSceneNode>(scene.newModel(*model, ModelUsage::GUI));

    SceneInitializer(scene)
        .aspect(aspect)
        .depth(kDefaultClipPlaneNear, 10.0f)
        .modelSupplier([&modelSceneNode](auto &_) { return modelSceneNode; })
        .modelScale(1.4f)
        .cameraFromModelNode("camerahook")
        .invoke();

    _lbl3dView->setSceneGraph(&scene);
}

void MainMenu::initModules() {
    auto items = vector<ListBox::Item>();
    for (auto &moduleName : _game.moduleNames()) {
        items.push_back(ListBox::Item {moduleName});
    }
    _lbModules->setItems(move(items));
}

bool MainMenu::handleClick(const Control &control) {
    if (control.tag() == "BTN_NEWGAME") {
        _game.startNewGame();
        return true;
    } else if (control.tag() == "BTN_EXIT") {
        _game.quit();
        return true;
    } else if (control.tag() == "BTN_WARP") {
        toggleWarpStage();
        return true;
    }
    return false;
}

bool MainMenu::handleListBoxItemClick(const ListBox &listBox, const ListBox::Item &item) {
    _game.warpToModule(item.text);
    return false;
}

void MainMenu::toggleWarpStage() {
    disableControl("LBL_MENUBG");
    disableControl("LBL_GAMELOGO");
    disableControl("BTN_NEWGAME");
    disableControl("BTN_LOADGAME");
    disableControl("BTN_MOVIES");
    disableControl("BTN_OPTIONS");
    disableControl("BTN_EXIT");
    disableControl("BTN_WARP");

    _lbl3dView->setEnabled(false);
    _lbModules->setEnabled(true);
}

} // namespace game

} // namespace reone
