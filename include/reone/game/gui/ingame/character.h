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

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"
#include "reone/gui/control/slider.h"
#include "reone/scene/node/model.h"

#include "../../gui.h"

namespace reone {

namespace game {

class InGameMenu;

class CharacterMenu : public GameGUI {
public:
    CharacterMenu(
        Game &game,
        InGameMenu &inGameMenu,
        ServicesView &services) :
        GameGUI(game, services),
        _inGameMenu(inGameMenu) {
        _resRef = guiResRef("character");
    }

    void update(float dt) override;

    void refreshControls();

private:
    static constexpr int kNumControlsGood = 10;
    static constexpr int kNumControlsBar = 6;

    struct Binding {
        std::shared_ptr<gui::Label> lbl3dChar;
        std::shared_ptr<gui::Button> btn3dChar;
        std::shared_ptr<gui::Slider> sldAlign;
        std::shared_ptr<gui::Label> lblStr;
        std::shared_ptr<gui::Label> lblFortitudeStat;
        std::shared_ptr<gui::Label> lblReflexStat;
        std::shared_ptr<gui::Label> lblWillStat;
        std::shared_ptr<gui::Label> lblDefenseStat;
        std::shared_ptr<gui::Label> lblForceStat;
        std::shared_ptr<gui::Label> lblVitalityStat;
        std::shared_ptr<gui::Label> lblDex;
        std::shared_ptr<gui::Label> lblCon;
        std::shared_ptr<gui::Label> lblInt;
        std::shared_ptr<gui::Label> lblCha;
        std::shared_ptr<gui::Label> lblWis;
        std::shared_ptr<gui::Label> lblStrMod;
        std::shared_ptr<gui::Label> lblDexMod;
        std::shared_ptr<gui::Label> lblConMod;
        std::shared_ptr<gui::Label> lblIntMod;
        std::shared_ptr<gui::Label> lblWisMod;
        std::shared_ptr<gui::Label> lblChaMod;
        std::shared_ptr<gui::Label> lblExperienceStat;
        std::shared_ptr<gui::Label> lblNeededXp;
        std::shared_ptr<gui::Label> lblStrength;
        std::shared_ptr<gui::Label> lblDexterity;
        std::shared_ptr<gui::Label> lblConstitution;
        std::shared_ptr<gui::Label> lblIntelligence;
        std::shared_ptr<gui::Label> lblCharisma;
        std::shared_ptr<gui::Label> lblReflex;
        std::shared_ptr<gui::Label> lblWill;
        std::shared_ptr<gui::Label> lblExperience;
        std::shared_ptr<gui::Label> lblNextLevel;
        std::shared_ptr<gui::Label> lblForce;
        std::shared_ptr<gui::Label> lblVitality;
        std::shared_ptr<gui::Label> lblDefense;
        std::shared_ptr<gui::Label> lblFortitude;
        std::shared_ptr<gui::Label> lblBevel;
        std::shared_ptr<gui::Label> lblWisdom;
        std::shared_ptr<gui::Label> lblBevel2;
        std::shared_ptr<gui::Label> lblLight;
        std::shared_ptr<gui::Label> lblDark;
        std::shared_ptr<gui::Button> btnExit;
        std::shared_ptr<gui::Button> btnAuto;
        std::shared_ptr<gui::Button> btnLevelup;
        std::shared_ptr<gui::Button> btnChange1;
        std::shared_ptr<gui::Button> btnChange2;

        // KOTOR only
        std::shared_ptr<gui::Label> lblAdorn;
        std::shared_ptr<gui::Button> btnScripts;
        std::shared_ptr<gui::Label> lblClass;
        std::shared_ptr<gui::Label> lblClass1;
        std::shared_ptr<gui::Label> lblClass2;
        std::shared_ptr<gui::Label> lblGood[kNumControlsGood];
        std::shared_ptr<gui::Label> lblLevel;
        std::shared_ptr<gui::Label> lblLevel1;
        std::shared_ptr<gui::Label> lblLevel2;
        std::shared_ptr<gui::Label> lblMore;
        std::shared_ptr<gui::Label> lblName;
        std::shared_ptr<gui::Button> btnCharLeft;
        std::shared_ptr<gui::Button> btnCharRight;
        // END KOTOR only

        // TSL only
        std::shared_ptr<gui::Label> lblForceMastery;
        std::shared_ptr<gui::Label> lblMoreBack;
        std::shared_ptr<gui::Label> lblStatsBorder;
        std::shared_ptr<gui::Label> lblTitle;
        std::shared_ptr<gui::Label> lblXpBack;
        std::shared_ptr<gui::Label> lblBar[kNumControlsBar];
        // END TSL only
    } _binding;

    InGameMenu &_inGameMenu;

    void onGUILoaded() override;

    void bindControls();

    void refreshPortraits();
    void refresh3D();

    std::shared_ptr<scene::ModelSceneNode> getSceneModel(scene::ISceneGraph &sceneGraph) const;
    std::string describeClass(ClassType clazz) const;
};

} // namespace game

} // namespace reone
