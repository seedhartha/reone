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

#pragma once

#include "../../gui/gui.h"

#include "../../resource/types.h"

#include "../object/spatial.h"

namespace reone {

namespace game {

class Game;

class Container : public gui::GUI {
public:
    Container(Game *game);

    void load() override;
    void open(const std::shared_ptr<SpatialObject> &contanier);

    SpatialObject &container() const;

private:
    Game *_game { nullptr };
    std::shared_ptr<SpatialObject> _container;

    void configureItemsListBox();
    void transferItemsToPlayer();

    std::shared_ptr<render::Texture> getItemFrameTexture(int stackSize) const;

    // Event handlers

    void onClick(const std::string &control) override;

    // END Event handlers
};

} // namespace game

} // namespace reone
