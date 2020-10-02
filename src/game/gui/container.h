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

#pragma once

#include "../../gui/gui.h"

#include "../../resource/types.h"

#include "../object/spatial.h"

namespace reone {

namespace game {

class ContainerGui : public gui::GUI {
public:
    ContainerGui(const render::GraphicsOptions &opts);

    void load(resource::GameVersion version);
    void open(SpatialObject *contanier);

    SpatialObject &container() const;

    void setOnGetItems(const std::function<void()> &fn);
    void setOnClose(const std::function<void()> &fn);

private:
    SpatialObject *_container { nullptr };

    std::function<void()> _onGetItems;
    std::function<void()> _onClose;

    void configureItemsListBox(resource::GameVersion version);
    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
