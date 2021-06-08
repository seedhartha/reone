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
#include "../../../gui/control/button.h"
#include "../../../gui/control/label.h"
#include "../../../gui/control/listbox.h"
#include "../../../gui/control/slider.h"
#include "../../../scene/node/modelnode.h"

namespace reone {

namespace game {

class InGameMenu;

class CharacterMenu : public GameGUI {
public:
    CharacterMenu(Game *game, InGameMenu &inGameMenu);

    void load() override;
    void update(float dt) override;

    void refreshControls();

private:
    struct Binding {
        std::shared_ptr<gui::Label> lbl3dChar {nullptr};
        std::shared_ptr<gui::Button> btn3dChar {nullptr};
        std::shared_ptr<gui::Slider> sldAlign {nullptr};
        std::shared_ptr<gui::Label> lblStr {nullptr};
        std::shared_ptr<gui::Label> lblFortitudeStat {nullptr};
        std::shared_ptr<gui::Label> lblReflexStat {nullptr};
        std::shared_ptr<gui::Label> lblWillStat {nullptr};
        std::shared_ptr<gui::Label> lblDefenseStat {nullptr};
        std::shared_ptr<gui::Label> lblForceStat {nullptr};
        std::shared_ptr<gui::Label> lblVitalityStat {nullptr};
        std::shared_ptr<gui::Label> lblDex {nullptr};
        std::shared_ptr<gui::Label> lblCon {nullptr};
        std::shared_ptr<gui::Label> lblInt {nullptr};
        std::shared_ptr<gui::Label> lblCha {nullptr};
        std::shared_ptr<gui::Label> lblWis {nullptr};
        std::shared_ptr<gui::Label> lblStrMod {nullptr};
        std::shared_ptr<gui::Label> lblDexMod {nullptr};
        std::shared_ptr<gui::Label> lblConMod {nullptr};
        std::shared_ptr<gui::Label> lblIntMod {nullptr};
        std::shared_ptr<gui::Label> lblWisMod {nullptr};
        std::shared_ptr<gui::Label> lblChaMod {nullptr};
        std::shared_ptr<gui::Label> lblExperienceStat {nullptr};
        std::shared_ptr<gui::Label> lblNeededXp {nullptr};
        std::shared_ptr<gui::Label> lblStrength {nullptr};
        std::shared_ptr<gui::Label> lblDexterity {nullptr};
        std::shared_ptr<gui::Label> lblConstitution {nullptr};
        std::shared_ptr<gui::Label> lblIntelligence {nullptr};
        std::shared_ptr<gui::Label> lblCharisma {nullptr};
        std::shared_ptr<gui::Label> lblReflex {nullptr};
        std::shared_ptr<gui::Label> lblWill {nullptr};
        std::shared_ptr<gui::Label> lblExperience {nullptr};
        std::shared_ptr<gui::Label> lblNextLevel {nullptr};
        std::shared_ptr<gui::Label> lblForce {nullptr};
        std::shared_ptr<gui::Label> lblVitality {nullptr};
        std::shared_ptr<gui::Label> lblDefense {nullptr};
        std::shared_ptr<gui::Label> lblFortitude {nullptr};
        std::shared_ptr<gui::Label> lblBevel {nullptr};
        std::shared_ptr<gui::Label> lblWisdom {nullptr};
        std::shared_ptr<gui::Label> lblBevel2 {nullptr};
        std::shared_ptr<gui::Label> lblLight {nullptr};
        std::shared_ptr<gui::Label> lblDark {nullptr};
        std::shared_ptr<gui::Button> btnExit {nullptr};
        std::shared_ptr<gui::Button> btnAuto {nullptr};
        std::shared_ptr<gui::Button> btnLevelup {nullptr};
        std::shared_ptr<gui::Button> btnChange1 {nullptr};
        std::shared_ptr<gui::Button> btnChange2 {nullptr};

        // KOTOR only
        std::shared_ptr<gui::Label> lblAdorn {nullptr};
        std::shared_ptr<gui::Button> btnScripts {nullptr};
        std::shared_ptr<gui::Label> lblClass {nullptr};
        std::shared_ptr<gui::Label> lblClass1 {nullptr};
        std::shared_ptr<gui::Label> lblClass2 {nullptr};
        std::shared_ptr<gui::Label> lblGood[10];
        std::shared_ptr<gui::Label> lblLevel {nullptr};
        std::shared_ptr<gui::Label> lblLevel1 {nullptr};
        std::shared_ptr<gui::Label> lblLevel2 {nullptr};
        std::shared_ptr<gui::Label> lblMore {nullptr};
        std::shared_ptr<gui::Label> lblName {nullptr};
        std::shared_ptr<gui::Button> btnCharLeft {nullptr};
        std::shared_ptr<gui::Button> btnCharRight {nullptr};
        // End KOTOR only

        // TSL only
        std::shared_ptr<gui::Label> lblForceMastery {nullptr};
        std::shared_ptr<gui::Label> lblMoreBack {nullptr};
        std::shared_ptr<gui::Label> lblStatsBorder {nullptr};
        std::shared_ptr<gui::Label> lblTitle {nullptr};
        std::shared_ptr<gui::Label> lblXpBack {nullptr};
        std::shared_ptr<gui::Label> lblBar[6];
        // End TSL only
    } _binding;

    InGameMenu &_inGameMenu;

    void bindControls();
    void refreshPortraits();
    void refresh3D();

    std::shared_ptr<scene::ModelSceneNode> getSceneModel(scene::SceneGraph &sceneGraph) const;
    std::string describeClass(ClassType clazz) const;

    void onClick(const std::string &control) override;
};

} // namespace game

} // namespace reone
