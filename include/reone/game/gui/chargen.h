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

#include "reone/gui/control/label.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/model.h"

#include "../character.h"
#include "../gui.h"

#include "chargen/abilities.h"
#include "chargen/classselect.h"
#include "chargen/custom.h"
#include "chargen/feats.h"
#include "chargen/levelup.h"
#include "chargen/nameentry.h"
#include "chargen/portraitselect.h"
#include "chargen/quick.h"
#include "chargen/quickorcustom.h"
#include "chargen/skills.h"

namespace reone {

namespace gui {

class Label;

}

namespace game {

enum class CharGenScreen {
    ClassSelection,
    QuickOrCustom,
    Quick,
    PortraitSelection,
    Name,
    Custom,
    Abilities,
    Skills,
    Feats,
    LevelUp
};

class CharacterGeneration : public GameGUI {
public:
    enum class Type {
        Quick,
        Custom,
        LevelUp
    };

    CharacterGeneration(Game &game, ServicesView &services);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;

    void startQuick();
    void startCustom();
    void startLevelUp();

    void finish();
    void cancel();

    void openClassSelection();
    void openNameEntry();
    void openAbilities();
    void openSkills();
    void openFeats();
    void openPortraitSelection();
    void openQuick();
    void openQuickOrCustom();
    void openCustom();
    void openSteps();
    void openLevelUp();

    void goToNextStep();

    const Character &character() const { return _character; }

    void setCharacter(Character character);

    const std::string &musicResRef() const {
        return _musicResRef;
    }

    const std::string &loadScreenResRef() const {
        return _loadScreenResRef;
    }

private:
    struct Controls {
        std::shared_ptr<gui::Label> CHA_AB_LBL;
        std::shared_ptr<gui::Label> CHA_LBL;
        std::shared_ptr<gui::Label> CON_AB_LBL;
        std::shared_ptr<gui::Label> CON_LBL;
        std::shared_ptr<gui::Label> DEF_ARROW_LBL;
        std::shared_ptr<gui::Label> DEX_AB_LBL;
        std::shared_ptr<gui::Label> DEX_LBL;
        std::shared_ptr<gui::Label> FORT_ARROW_LBL;
        std::shared_ptr<gui::Label> INT_AB_LBL;
        std::shared_ptr<gui::Label> INT_LBL;
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BEVEL_L;
        std::shared_ptr<gui::Label> LBL_BEVEL_M;
        std::shared_ptr<gui::Label> LBL_BEVEL_R;
        std::shared_ptr<gui::Label> LBL_CLASS;
        std::shared_ptr<gui::Label> LBL_DEF;
        std::shared_ptr<gui::Label> LBL_DEF_NAME;
        std::shared_ptr<gui::Label> LBL_FORTITUDE;
        std::shared_ptr<gui::Label> LBL_LEVEL;
        std::shared_ptr<gui::Label> LBL_LEVEL_VAL;
        std::shared_ptr<gui::Label> LBL_NAME;
        std::shared_ptr<gui::Label> LBL_PORTBORDER;
        std::shared_ptr<gui::Label> LBL_REFLEX;
        std::shared_ptr<gui::Label> LBL_STATSBACK;
        std::shared_ptr<gui::Label> LBL_STATSBORDER;
        std::shared_ptr<gui::Label> LBL_VIT;
        std::shared_ptr<gui::Label> LBL_VIT_NAME;
        std::shared_ptr<gui::Label> LBL_WILL;
        std::shared_ptr<gui::Label> MAIN_TITLE_LBL;
        std::shared_ptr<gui::Label> MODEL_LBL;
        std::shared_ptr<gui::Label> NEW_DEF_LBL;
        std::shared_ptr<gui::Label> NEW_FORT_LBL;
        std::shared_ptr<gui::Label> NEW_LBL;
        std::shared_ptr<gui::Label> NEW_REFL_LBL;
        std::shared_ptr<gui::Label> NEW_VIT_LBL;
        std::shared_ptr<gui::Label> NEW_WILL_LBL;
        std::shared_ptr<gui::Label> OLD_DEF_LBL;
        std::shared_ptr<gui::Label> OLD_FORT_LBL;
        std::shared_ptr<gui::Label> OLD_LBL;
        std::shared_ptr<gui::Label> OLD_REFL_LBL;
        std::shared_ptr<gui::Label> OLD_VIT_LBL;
        std::shared_ptr<gui::Label> OLD_WILL_LBL;
        std::shared_ptr<gui::Label> PORTRAIT_LBL;
        std::shared_ptr<gui::Label> REFL_ARROW_LBL;
        std::shared_ptr<gui::Label> STR_AB_LBL;
        std::shared_ptr<gui::Label> STR_LBL;
        std::shared_ptr<gui::Label> VIT_ARROW_LBL;
        std::shared_ptr<gui::Label> WILL_ARROW_LBL;
        std::shared_ptr<gui::Label> WIS_AB_LBL;
        std::shared_ptr<gui::Label> WIS_LBL;
    };

    Controls _controls;

    std::string _musicResRef;
    std::string _loadScreenResRef;

    CharGenScreen _screen {CharGenScreen::ClassSelection};
    Type _type {Type::Quick};
    Character _character;

    // Sub GUI

    std::unique_ptr<ClassSelection> _classSelection;
    std::unique_ptr<QuickOrCustom> _quickOrCustom;
    std::unique_ptr<QuickCharacterGeneration> _quick;
    std::unique_ptr<CustomCharacterGeneration> _custom;
    std::unique_ptr<PortraitSelection> _portraitSelection;
    std::unique_ptr<CharGenAbilities> _abilities;
    std::unique_ptr<CharGenSkills> _charGenSkills;
    std::unique_ptr<CharGenFeats> _charGenFeats;
    std::unique_ptr<NameEntry> _nameEntry;
    std::unique_ptr<LevelUpMenu> _levelUp;

    // END Sub GUI

    void onGUILoaded() override;

    void bindControls() {
        _controls.CHA_AB_LBL = findControl<gui::Label>("CHA_AB_LBL");
        _controls.CHA_LBL = findControl<gui::Label>("CHA_LBL");
        _controls.CON_AB_LBL = findControl<gui::Label>("CON_AB_LBL");
        _controls.CON_LBL = findControl<gui::Label>("CON_LBL");
        _controls.DEF_ARROW_LBL = findControl<gui::Label>("DEF_ARROW_LBL");
        _controls.DEX_AB_LBL = findControl<gui::Label>("DEX_AB_LBL");
        _controls.DEX_LBL = findControl<gui::Label>("DEX_LBL");
        _controls.FORT_ARROW_LBL = findControl<gui::Label>("FORT_ARROW_LBL");
        _controls.INT_AB_LBL = findControl<gui::Label>("INT_AB_LBL");
        _controls.INT_LBL = findControl<gui::Label>("INT_LBL");
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BEVEL_L = findControl<gui::Label>("LBL_BEVEL_L");
        _controls.LBL_BEVEL_M = findControl<gui::Label>("LBL_BEVEL_M");
        _controls.LBL_BEVEL_R = findControl<gui::Label>("LBL_BEVEL_R");
        _controls.LBL_CLASS = findControl<gui::Label>("LBL_CLASS");
        _controls.LBL_DEF = findControl<gui::Label>("LBL_DEF");
        _controls.LBL_DEF_NAME = findControl<gui::Label>("LBL_DEF_NAME");
        _controls.LBL_FORTITUDE = findControl<gui::Label>("LBL_FORTITUDE");
        _controls.LBL_LEVEL = findControl<gui::Label>("LBL_LEVEL");
        _controls.LBL_LEVEL_VAL = findControl<gui::Label>("LBL_LEVEL_VAL");
        _controls.LBL_NAME = findControl<gui::Label>("LBL_NAME");
        _controls.LBL_PORTBORDER = findControl<gui::Label>("LBL_PORTBORDER");
        _controls.LBL_REFLEX = findControl<gui::Label>("LBL_REFLEX");
        _controls.LBL_STATSBACK = findControl<gui::Label>("LBL_STATSBACK");
        _controls.LBL_STATSBORDER = findControl<gui::Label>("LBL_STATSBORDER");
        _controls.LBL_VIT = findControl<gui::Label>("LBL_VIT");
        _controls.LBL_VIT_NAME = findControl<gui::Label>("LBL_VIT_NAME");
        _controls.LBL_WILL = findControl<gui::Label>("LBL_WILL");
        _controls.MAIN_TITLE_LBL = findControl<gui::Label>("MAIN_TITLE_LBL");
        _controls.MODEL_LBL = findControl<gui::Label>("MODEL_LBL");
        _controls.NEW_DEF_LBL = findControl<gui::Label>("NEW_DEF_LBL");
        _controls.NEW_FORT_LBL = findControl<gui::Label>("NEW_FORT_LBL");
        _controls.NEW_LBL = findControl<gui::Label>("NEW_LBL");
        _controls.NEW_REFL_LBL = findControl<gui::Label>("NEW_REFL_LBL");
        _controls.NEW_VIT_LBL = findControl<gui::Label>("NEW_VIT_LBL");
        _controls.NEW_WILL_LBL = findControl<gui::Label>("NEW_WILL_LBL");
        _controls.OLD_DEF_LBL = findControl<gui::Label>("OLD_DEF_LBL");
        _controls.OLD_FORT_LBL = findControl<gui::Label>("OLD_FORT_LBL");
        _controls.OLD_LBL = findControl<gui::Label>("OLD_LBL");
        _controls.OLD_REFL_LBL = findControl<gui::Label>("OLD_REFL_LBL");
        _controls.OLD_VIT_LBL = findControl<gui::Label>("OLD_VIT_LBL");
        _controls.OLD_WILL_LBL = findControl<gui::Label>("OLD_WILL_LBL");
        _controls.PORTRAIT_LBL = findControl<gui::Label>("PORTRAIT_LBL");
        _controls.REFL_ARROW_LBL = findControl<gui::Label>("REFL_ARROW_LBL");
        _controls.STR_AB_LBL = findControl<gui::Label>("STR_AB_LBL");
        _controls.STR_LBL = findControl<gui::Label>("STR_LBL");
        _controls.VIT_ARROW_LBL = findControl<gui::Label>("VIT_ARROW_LBL");
        _controls.WILL_ARROW_LBL = findControl<gui::Label>("WILL_ARROW_LBL");
        _controls.WIS_AB_LBL = findControl<gui::Label>("WIS_AB_LBL");
        _controls.WIS_LBL = findControl<gui::Label>("WIS_LBL");
    }

    void reloadCharacterModel();
    void updateAttributes();
    void changeScreen(CharGenScreen screen);

    GameGUI *getSubGUI() const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(scene::ISceneGraph &sceneGraph);

    void setAttributesVisible(bool visible);

    // Loading

    void loadClassSelection();
    void loadQuickOrCustom();
    void loadQuick();
    void loadPortraitSelection();
    void loadNameEntry();
    void loadCustom();
    void loadAbilities();
    void loadSkills();
    void loadFeats();
    void loadLevelUp();

    // END Loading
};

} // namespace game

} // namespace reone
