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
#include "reone/scene/graph.h"
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

    struct Controls {
        std::shared_ptr<gui::Button> BTN_3DCHAR;
        std::shared_ptr<gui::Button> BTN_AUTO;
        std::shared_ptr<gui::Button> BTN_CHANGE1;
        std::shared_ptr<gui::Button> BTN_CHANGE2;
        std::shared_ptr<gui::Button> BTN_EXIT;
        std::shared_ptr<gui::Button> BTN_LEVELUP;
        std::shared_ptr<gui::Button> BTN_SCRIPTS;
        std::shared_ptr<gui::Label> LBL_3DCHAR;
        std::shared_ptr<gui::Label> LBL_ADORN;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BAR6;
        std::shared_ptr<gui::Label> LBL_BEVEL;
        std::shared_ptr<gui::Label> LBL_BEVEL2;
        std::shared_ptr<gui::Label> LBL_CHA;
        std::shared_ptr<gui::Label> LBL_CHARISMA;
        std::shared_ptr<gui::Label> LBL_CHA_MOD;
        std::shared_ptr<gui::Label> LBL_CLASS;
        std::shared_ptr<gui::Label> LBL_CLASS1;
        std::shared_ptr<gui::Label> LBL_CLASS2;
        std::shared_ptr<gui::Label> LBL_CON;
        std::shared_ptr<gui::Label> LBL_CONSTITUTION;
        std::shared_ptr<gui::Label> LBL_CON_MOD;
        std::shared_ptr<gui::Label> LBL_DARK;
        std::shared_ptr<gui::Label> LBL_DEFENSE;
        std::shared_ptr<gui::Label> LBL_DEFENSE_STAT;
        std::shared_ptr<gui::Label> LBL_DEX;
        std::shared_ptr<gui::Label> LBL_DEXTERITY;
        std::shared_ptr<gui::Label> LBL_DEX_MOD;
        std::shared_ptr<gui::Label> LBL_EXPERIENCE;
        std::shared_ptr<gui::Label> LBL_EXPERIENCE_STAT;
        std::shared_ptr<gui::Label> LBL_FORCE;
        std::shared_ptr<gui::Label> LBL_FORCEMASTERY;
        std::shared_ptr<gui::Label> LBL_FORCE_STAT;
        std::shared_ptr<gui::Label> LBL_FORTITUDE;
        std::shared_ptr<gui::Label> LBL_FORTITUDE_STAT;
        std::shared_ptr<gui::Button> LBL_GOOD1;
        std::shared_ptr<gui::Button> LBL_GOOD10;
        std::shared_ptr<gui::Button> LBL_GOOD2;
        std::shared_ptr<gui::Button> LBL_GOOD3;
        std::shared_ptr<gui::Button> LBL_GOOD4;
        std::shared_ptr<gui::Button> LBL_GOOD5;
        std::shared_ptr<gui::Button> LBL_GOOD6;
        std::shared_ptr<gui::Button> LBL_GOOD7;
        std::shared_ptr<gui::Button> LBL_GOOD8;
        std::shared_ptr<gui::Button> LBL_GOOD9;
        std::shared_ptr<gui::Label> LBL_INT;
        std::shared_ptr<gui::Label> LBL_INTELLIGENCE;
        std::shared_ptr<gui::Label> LBL_INT_MOD;
        std::shared_ptr<gui::Label> LBL_LEVEL;
        std::shared_ptr<gui::Label> LBL_LEVEL1;
        std::shared_ptr<gui::Label> LBL_LEVEL2;
        std::shared_ptr<gui::Label> LBL_LIGHT;
        std::shared_ptr<gui::Label> LBL_MORE;
        std::shared_ptr<gui::Label> LBL_MORE_BACK;
        std::shared_ptr<gui::Label> LBL_NAME;
        std::shared_ptr<gui::Label> LBL_NEEDED_XP;
        std::shared_ptr<gui::Label> LBL_NEXT_LEVEL;
        std::shared_ptr<gui::Label> LBL_REFLEX;
        std::shared_ptr<gui::Label> LBL_REFLEX_STAT;
        std::shared_ptr<gui::Label> LBL_STATSBORDER;
        std::shared_ptr<gui::Label> LBL_STR;
        std::shared_ptr<gui::Label> LBL_STRENGTH;
        std::shared_ptr<gui::Label> LBL_STR_MOD;
        std::shared_ptr<gui::Label> LBL_TITLE;
        std::shared_ptr<gui::Label> LBL_VITALITY;
        std::shared_ptr<gui::Label> LBL_VITALITY_STAT;
        std::shared_ptr<gui::Label> LBL_WILL;
        std::shared_ptr<gui::Label> LBL_WILL_STAT;
        std::shared_ptr<gui::Label> LBL_WIS;
        std::shared_ptr<gui::Label> LBL_WISDOM;
        std::shared_ptr<gui::Label> LBL_WIS_MOD;
        std::shared_ptr<gui::Label> LBL_XP_BACK;
        std::shared_ptr<gui::Slider> SLD_ALIGN;
    };

    Controls _controls;

    std::vector<std::shared_ptr<gui::Button>> _lblGood;
    std::vector<std::shared_ptr<gui::Label>> _lblBar;

    InGameMenu &_inGameMenu;

    void onGUILoaded() override;

    void bindControls() {
        _controls.BTN_3DCHAR = findControl<gui::Button>("BTN_3DCHAR");
        _controls.BTN_AUTO = findControl<gui::Button>("BTN_AUTO");
        _controls.BTN_CHANGE1 = findControl<gui::Button>("BTN_CHANGE1");
        _controls.BTN_CHANGE2 = findControl<gui::Button>("BTN_CHANGE2");
        _controls.BTN_EXIT = findControl<gui::Button>("BTN_EXIT");
        _controls.BTN_LEVELUP = findControl<gui::Button>("BTN_LEVELUP");
        _controls.BTN_SCRIPTS = findControl<gui::Button>("BTN_SCRIPTS");
        _controls.LBL_3DCHAR = findControl<gui::Label>("LBL_3DCHAR");
        _controls.LBL_ADORN = findControl<gui::Label>("LBL_ADORN");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BAR6 = findControl<gui::Label>("LBL_BAR6");
        _controls.LBL_BEVEL = findControl<gui::Label>("LBL_BEVEL");
        _controls.LBL_BEVEL2 = findControl<gui::Label>("LBL_BEVEL2");
        _controls.LBL_CHA = findControl<gui::Label>("LBL_CHA");
        _controls.LBL_CHARISMA = findControl<gui::Label>("LBL_CHARISMA");
        _controls.LBL_CHA_MOD = findControl<gui::Label>("LBL_CHA_MOD");
        _controls.LBL_CLASS = findControl<gui::Label>("LBL_CLASS");
        _controls.LBL_CLASS1 = findControl<gui::Label>("LBL_CLASS1");
        _controls.LBL_CLASS2 = findControl<gui::Label>("LBL_CLASS2");
        _controls.LBL_CON = findControl<gui::Label>("LBL_CON");
        _controls.LBL_CONSTITUTION = findControl<gui::Label>("LBL_CONSTITUTION");
        _controls.LBL_CON_MOD = findControl<gui::Label>("LBL_CON_MOD");
        _controls.LBL_DARK = findControl<gui::Label>("LBL_DARK");
        _controls.LBL_DEFENSE = findControl<gui::Label>("LBL_DEFENSE");
        _controls.LBL_DEFENSE_STAT = findControl<gui::Label>("LBL_DEFENSE_STAT");
        _controls.LBL_DEX = findControl<gui::Label>("LBL_DEX");
        _controls.LBL_DEXTERITY = findControl<gui::Label>("LBL_DEXTERITY");
        _controls.LBL_DEX_MOD = findControl<gui::Label>("LBL_DEX_MOD");
        _controls.LBL_EXPERIENCE = findControl<gui::Label>("LBL_EXPERIENCE");
        _controls.LBL_EXPERIENCE_STAT = findControl<gui::Label>("LBL_EXPERIENCE_STAT");
        _controls.LBL_FORCE = findControl<gui::Label>("LBL_FORCE");
        _controls.LBL_FORCEMASTERY = findControl<gui::Label>("LBL_FORCEMASTERY");
        _controls.LBL_FORCE_STAT = findControl<gui::Label>("LBL_FORCE_STAT");
        _controls.LBL_FORTITUDE = findControl<gui::Label>("LBL_FORTITUDE");
        _controls.LBL_FORTITUDE_STAT = findControl<gui::Label>("LBL_FORTITUDE_STAT");
        _controls.LBL_GOOD1 = findControl<gui::Button>("LBL_GOOD1");
        _controls.LBL_GOOD10 = findControl<gui::Button>("LBL_GOOD10");
        _controls.LBL_GOOD2 = findControl<gui::Button>("LBL_GOOD2");
        _controls.LBL_GOOD3 = findControl<gui::Button>("LBL_GOOD3");
        _controls.LBL_GOOD4 = findControl<gui::Button>("LBL_GOOD4");
        _controls.LBL_GOOD5 = findControl<gui::Button>("LBL_GOOD5");
        _controls.LBL_GOOD6 = findControl<gui::Button>("LBL_GOOD6");
        _controls.LBL_GOOD7 = findControl<gui::Button>("LBL_GOOD7");
        _controls.LBL_GOOD8 = findControl<gui::Button>("LBL_GOOD8");
        _controls.LBL_GOOD9 = findControl<gui::Button>("LBL_GOOD9");
        _controls.LBL_INT = findControl<gui::Label>("LBL_INT");
        _controls.LBL_INTELLIGENCE = findControl<gui::Label>("LBL_INTELLIGENCE");
        _controls.LBL_INT_MOD = findControl<gui::Label>("LBL_INT_MOD");
        _controls.LBL_LEVEL = findControl<gui::Label>("LBL_LEVEL");
        _controls.LBL_LEVEL1 = findControl<gui::Label>("LBL_LEVEL1");
        _controls.LBL_LEVEL2 = findControl<gui::Label>("LBL_LEVEL2");
        _controls.LBL_LIGHT = findControl<gui::Label>("LBL_LIGHT");
        _controls.LBL_MORE = findControl<gui::Label>("LBL_MORE");
        _controls.LBL_MORE_BACK = findControl<gui::Label>("LBL_MORE_BACK");
        _controls.LBL_NAME = findControl<gui::Label>("LBL_NAME");
        _controls.LBL_NEEDED_XP = findControl<gui::Label>("LBL_NEEDED_XP");
        _controls.LBL_NEXT_LEVEL = findControl<gui::Label>("LBL_NEXT_LEVEL");
        _controls.LBL_REFLEX = findControl<gui::Label>("LBL_REFLEX");
        _controls.LBL_REFLEX_STAT = findControl<gui::Label>("LBL_REFLEX_STAT");
        _controls.LBL_STATSBORDER = findControl<gui::Label>("LBL_STATSBORDER");
        _controls.LBL_STR = findControl<gui::Label>("LBL_STR");
        _controls.LBL_STRENGTH = findControl<gui::Label>("LBL_STRENGTH");
        _controls.LBL_STR_MOD = findControl<gui::Label>("LBL_STR_MOD");
        _controls.LBL_TITLE = findControl<gui::Label>("LBL_TITLE");
        _controls.LBL_VITALITY = findControl<gui::Label>("LBL_VITALITY");
        _controls.LBL_VITALITY_STAT = findControl<gui::Label>("LBL_VITALITY_STAT");
        _controls.LBL_WILL = findControl<gui::Label>("LBL_WILL");
        _controls.LBL_WILL_STAT = findControl<gui::Label>("LBL_WILL_STAT");
        _controls.LBL_WIS = findControl<gui::Label>("LBL_WIS");
        _controls.LBL_WISDOM = findControl<gui::Label>("LBL_WISDOM");
        _controls.LBL_WIS_MOD = findControl<gui::Label>("LBL_WIS_MOD");
        _controls.LBL_XP_BACK = findControl<gui::Label>("LBL_XP_BACK");
        _controls.SLD_ALIGN = findControl<gui::Slider>("SLD_ALIGN");
    }

    void refreshPortraits();
    void refresh3D();

    std::shared_ptr<scene::ModelSceneNode> getSceneModel(scene::ISceneGraph &sceneGraph) const;
    std::string describeClass(ClassType clazz) const;
};

} // namespace game

} // namespace reone
