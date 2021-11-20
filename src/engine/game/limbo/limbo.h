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

#include "../core/game.h"

namespace reone {

namespace game {

class Limbo : public Game {
public:
    Limbo(
        boost::filesystem::path path,
        Options options,
        Services &services) :
        Game(
            false,
            std::move(path),
            std::move(options),
            services) {
    }

private:
    void initResourceProviders() override;
    void initRoutines() override;

    void start() override;

    void loadModuleNames() override;
    void loadModuleResources(const std::string &moduleName) override;

    void loadInGameMenus() override {}
    void loadLoadingScreen() override {}

    void openMainMenu() override {}
    void openInGame() override;
    void openContainer(const std::shared_ptr<SpatialObject> &container) override {}
    void openPartySelection(const PartySelectionContext &ctx) override {}
    void openSaveLoad(SaveLoadMode mode) override {}
    void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef) override {}
    void pauseConversation() override {}
    void resumeConversation() override {}
    void changeScreen(GameScreen screen) override;

    void onModuleSelected(const std::string &name) {}
    void drawHUD() {}

    gui::GUI *getScreenGUI() const { return nullptr; }
    CameraType getConversationCamera(int &cameraId) const { return CameraType::FirstPerson; };
};

} // namespace game

} // namespace reone
