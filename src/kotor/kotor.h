
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

#include "../game/game.h"

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

constexpr char kKeyFilename[] = "chitin.key";
constexpr char kTexturePackDirectoryName[] = "texturepacks";
constexpr char kGUITexturePackFilename[] = "swpc_tex_gui.erf";
constexpr char kTexturePackFilename[] = "swpc_tex_tpa.erf";
constexpr char kMusicDirectoryName[] = "streammusic";
constexpr char kSoundsDirectoryName[] = "streamsounds";
constexpr char kLipsDirectoryName[] = "lips";
constexpr char kOverrideDirectoryName[] = "override";

class KotOR : public game::Game {
public:
    KotOR(
        boost::filesystem::path path,
        game::Options options,
        game::Services &services);

    void initResourceProviders() override;

    void openMainMenu() override;
    void openSaveLoad(game::SaveLoadMode mode) override;
    void openInGame() override;
    void openInGameMenu(game::InGameMenuTab tab);
    void openContainer(const std::shared_ptr<game::SpatialObject> &container) override;
    void openPartySelection(const game::PartySelectionContext &ctx) override;
    void openLevelUp();

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<game::SpatialObject> &owner, const std::string &resRef) override;

    void resumeConversation() override;
    void pauseConversation() override;

    virtual bool isTSL() const { return false; }

    void setBarkBubbleText(std::string text, float durartion);

    static void fillScriptRoutines(game::IRoutines &routines);

protected:
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

    void initScriptRoutines() override;
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
