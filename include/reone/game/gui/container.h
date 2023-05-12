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
    ContainerGUI(Game &game, ServicesView &services);

    void load() override;
    void open(std::shared_ptr<Object> contanier);

    Object &container() const { return *_container; }

private:
    struct Binding {
        std::shared_ptr<gui::Label> lblMessage;
        std::shared_ptr<gui::ListBox> lbItems;
        std::shared_ptr<gui::Button> btnOk;
        std::shared_ptr<gui::Button> btnGiveItems;
        std::shared_ptr<gui::Button> btnCancel;
    } _binding;

    std::shared_ptr<Object> _container;

    void bindControls();
    void configureItemsListBox();
    void transferItemsToPlayer();

    std::shared_ptr<graphics::Texture> getItemFrameTexture(int stackSize) const;
};

} // namespace game

} // namespace reone
