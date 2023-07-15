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

#pragma once

#include "../gui.h"

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"

#include "../object.h"

namespace reone {

namespace game {

class ContainerGUI : public GameGUI {
public:
    ContainerGUI(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("container");
    }

    void open(std::shared_ptr<Object> contanier);

    Object &container() const { return *_container; }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_CANCEL;
        std::shared_ptr<gui::Button> BTN_GIVEITEMS;
        std::shared_ptr<gui::Button> BTN_OK;
        std::shared_ptr<gui::Label> LBL_MESSAGE;
        std::shared_ptr<gui::ListBox> LB_ITEMS;
    };

    Controls _controls;

    std::shared_ptr<Object> _container;

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_CANCEL = findControl<gui::Button>("BTN_CANCEL");
        _controls.BTN_GIVEITEMS = findControl<gui::Button>("BTN_GIVEITEMS");
        _controls.BTN_OK = findControl<gui::Button>("BTN_OK");
        _controls.LBL_MESSAGE = findControl<gui::Label>("LBL_MESSAGE");
        _controls.LB_ITEMS = findControl<gui::ListBox>("LB_ITEMS");
    }

    void configureItemsListBox();
    void transferItemsToPlayer();

    std::shared_ptr<graphics::Texture> getItemFrameTexture(int stackSize) const;
};

} // namespace game

} // namespace reone
