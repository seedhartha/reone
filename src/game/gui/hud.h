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

#include "../../resource/types.h"

#include "barkbubble.h"
#include "debugoverlay.h"
#include "gui.h"
#include "selectoverlay.h"

namespace reone {

namespace game {

class Game;

class HUD : public GameGUI {
public:
    HUD(Game *game);

    void load() override;

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void render() override;

    void onClick(const std::string &control) override;

    BarkBubble &barkBubble() const { return *_barkBubble; }

private:
    Game *_game { nullptr };
    SelectionOverlay _select;
    DebugOverlay _debug;
    std::unique_ptr<BarkBubble> _barkBubble;

    void showCombatHud();
    void hideCombatHud();
    void refreshActionQueueItems() const;

    void drawHealth(int memberIndex);
    void drawMinimap();
};

} // namespace game

} // namespace reone
