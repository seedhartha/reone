
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

#include "../game.h"

#include "gui/chargen.h"
#include "gui/computer.h"
#include "gui/container.h"
#include "gui/conversation.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/ingame.h"
#include "gui/mainmenu.h"
#include "gui/partyselect.h"
#include "gui/saveload.h"

namespace reone {

namespace kotor {

class KotOR : public game::Game {
public:
    KotOR(
        bool tsl,
        boost::filesystem::path path,
        game::Options &options,
        game::GameServices &services) :
        Game(
            std::move(path),
            options,
            services),
        _tsl(tsl) {
    }

    void initResourceProviders() override;

    void init() override;

    void openMainMenu() override;
    void openSaveLoad(game::SaveLoadMode mode) override;
    void openInGame() override;
    void openInGameMenu(game::InGameMenuTab tab);
    void openContainer(const std::shared_ptr<game::Object> &container) override;
    void openPartySelection(const game::PartySelectionContext &ctx) override;
    void openLevelUp();

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<game::Object> &owner, const std::string &resRef) override;

    void resumeConversation() override;
    void pauseConversation() override;

    bool isTSL() const { return _tsl; }

    void setBarkBubbleText(std::string text, float durartion);

private:
    bool _tsl;

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<CharacterGeneration> _charGen;
    std::unique_ptr<HUD> _hud;
    std::unique_ptr<InGameMenu> _inGame;
    std::unique_ptr<DialogGUI> _dialog;
    std::unique_ptr<ComputerGUI> _computer;
    std::unique_ptr<ContainerGUI> _container;
    std::unique_ptr<PartySelection> _partySelect;
    std::unique_ptr<SaveLoad> _saveLoad;

    Conversation *_conversation {nullptr}; /**< pointer to either DialogGUI or ComputerGUI  */

    // END GUI

    void start() override;

    void loadModuleNames() override;
    void loadModuleResources(const std::string &moduleName) override;

    void loadInGameMenus() override;
    void loadMainMenu();
    void loadLoadingScreen() override;
    void loadCharacterGeneration();
    void loadHUD();
    void loadInGame();
    void loadDialog();
    void loadComputer();
    void loadContainer();
    void loadPartySelection();
    void loadSaveLoad();

    void onModuleSelected(const std::string &name) override;
    void drawHUD() override;

    void changeScreen(GameScreen screen) override;

    void getDefaultPartyMembers(std::string &member1, std::string &member2, std::string &member3) const override;
    gui::GUI *getScreenGUI() const override;
    game::CameraType getConversationCamera(int &cameraId) const override;
};

} // namespace kotor

} // namespace reone
