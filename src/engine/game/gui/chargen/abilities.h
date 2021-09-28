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

#include "../../d20/attributes.h"

#include "../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace game {

class CharacterGeneration;

class CharGenAbilities : public GameGUI {
public:
    CharGenAbilities(
        CharacterGeneration *charGen,
        Game *game,
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
        resource::Resources &resources,
        resource::Strings &strings);

    void load() override;

    void reset(bool newGame);

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnAccept;
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnRecommended;
        std::shared_ptr<gui::Label> costPointsLbl;
        std::shared_ptr<gui::Label> remainingSelectionsLbl;
        std::shared_ptr<gui::ListBox> lbDesc;

        std::shared_ptr<gui::Label> strLbl;
        std::shared_ptr<gui::Label> dexLbl;
        std::shared_ptr<gui::Label> conLbl;
        std::shared_ptr<gui::Label> intLbl;
        std::shared_ptr<gui::Label> wisLbl;
        std::shared_ptr<gui::Label> chaLbl;

        std::shared_ptr<gui::Button> strPointsBtn;
        std::shared_ptr<gui::Button> dexPointsBtn;
        std::shared_ptr<gui::Button> conPointsBtn;
        std::shared_ptr<gui::Button> intPointsBtn;
        std::shared_ptr<gui::Button> wisPointsBtn;
        std::shared_ptr<gui::Button> chaPointsBtn;

        std::shared_ptr<gui::Button> strMinusBtn;
        std::shared_ptr<gui::Button> dexMinusBtn;
        std::shared_ptr<gui::Button> conMinusBtn;
        std::shared_ptr<gui::Button> intMinusBtn;
        std::shared_ptr<gui::Button> wisMinusBtn;
        std::shared_ptr<gui::Button> chaMinusBtn;

        std::shared_ptr<gui::Button> strPlusBtn;
        std::shared_ptr<gui::Button> dexPlusBtn;
        std::shared_ptr<gui::Button> conPlusBtn;
        std::shared_ptr<gui::Button> intPlusBtn;
        std::shared_ptr<gui::Button> wisPlusBtn;
        std::shared_ptr<gui::Button> chaPlusBtn;

        // KotOR only
        std::shared_ptr<gui::Label> lblAbilityMod;
        // END KotOR only
    } _binding;

    CharacterGeneration *_charGen;
    CreatureAttributes _attributes;
    int _points {0};

    void bindControls();
    void refreshControls();
    void updateCharacter();

    int getPointCost(Ability ability) const;

    void onAbilityLabelFocusChanged(Ability ability, bool focus);
    void onMinusButtonClick(Ability ability);
    void onPlusButtonClick(Ability ability);
};

} // namespace game

} // namespace reone
