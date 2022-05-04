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
#include "../../gui/sceneinitializer.h"
#include "../../scene/graphs.h"
#include "../../scene/node/model.h"
#include "../../scene/services.h"

#include "../types.h"

#include "game.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::scene;

namespace reone {

namespace game {

void MainMenu::init() {
    load("mainmenu16x12");
    bindControls();

    disableControl("BTN_WARP");
    disableControl("LB_MODULES");
    disableControl("LBL_BW");
    disableControl("LBL_LUCAS");
    disableControl("LBL_NEWCONTENT");

    // Init 3D view

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

void MainMenu::bindControls() {
    _lbl3dView = findControl<Label>("LBL_3DVIEW");
}

bool MainMenu::handleClick(const Control &control) {
    if (control.tag() == "BTN_NEWGAME") {
        _game.startNewGame();
        return true;
    } else if (control.tag() == "BTN_EXIT") {
        _game.quit();
        return true;
    }
    return false;
}

} // namespace game

} // namespace reone
