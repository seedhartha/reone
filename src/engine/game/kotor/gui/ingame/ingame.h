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

#include "../gui.h"

#include "abilities.h"
#include "character.h"
#include "equip.h"
#include "inventory.h"
#include "journal.h"
#include "map.h"
#include "messages.h"
#include "options.h"

namespace reone {

namespace game {

class InGameMenu : public GameGUI {
public:
    InGameMenu(
        KotOR *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Fonts &fonts,
        graphics::Lips &lips,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        resource::Gffs &gffs,
        resource::Resources &resources,
        resource::Strings &strings,
        resource::TwoDas &twoDas);

    void load() override;
    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;
    void draw3D() override;

    void openEquipment();
    void openInventory();
    void openCharacter();
    void openAbilities();
    void openMessages();
    void openJournal();
    void openMap();
    void openOptions();

    std::shared_ptr<gui::Button> getBtnChange2();
    std::shared_ptr<gui::Button> getBtnChange3();

private:
    struct Binding {
        std::shared_ptr<gui::Label> lblhEqu;
        std::shared_ptr<gui::Label> lblhInv;
        std::shared_ptr<gui::Label> lblhCha;
        std::shared_ptr<gui::Label> lblhAbi;
        std::shared_ptr<gui::Label> lblhMsg;
        std::shared_ptr<gui::Label> lblhJou;
        std::shared_ptr<gui::Label> lblhMap;
        std::shared_ptr<gui::Label> lblhOpt;

        std::shared_ptr<gui::Button> btnEqu;
        std::shared_ptr<gui::Button> btnInv;
        std::shared_ptr<gui::Button> btnChar;
        std::shared_ptr<gui::Button> btnAbi;
        std::shared_ptr<gui::Button> btnMsg;
        std::shared_ptr<gui::Button> btnJou;
        std::shared_ptr<gui::Button> btnMap;
        std::shared_ptr<gui::Button> btnOpt;
    } _binding;

    InGameMenuTab _tab {InGameMenuTab::None};

    std::unique_ptr<CharacterMenu> _character;
    std::unique_ptr<Equipment> _equip;
    std::unique_ptr<InventoryMenu> _inventory;
    std::unique_ptr<AbilitiesMenu> _abilities;
    std::unique_ptr<MessagesMenu> _messages;
    std::unique_ptr<JournalMenu> _journal;
    std::unique_ptr<MapMenu> _map;
    std::unique_ptr<OptionsMenu> _options;

    void bindControls();
    void updateTabButtons();
    void changeTab(InGameMenuTab tab);

    void loadCharacter();
    void loadEquipment();
    void loadInventory();
    void loadAbilities();
    void loadMessages();
    void loadJournal();
    void loadMap();
    void loadOptions();

    GUI *getActiveTabGUI() const;

    void setTabLabelsFocusable(bool focusable);
};

} // namespace game

} // namespace reone
