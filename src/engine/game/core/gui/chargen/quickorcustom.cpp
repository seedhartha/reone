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

#include "quickorcustom.h"

#include "../../../../gui/control/listbox.h"
#include "../../../../resource/strings.h"

#include "../../game.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefQuickHelpText = 241;
static constexpr int kStrRefCustomHelpText = 242;

QuickOrCustom::QuickOrCustom(
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
    resource::Strings &strings) :
    GameGUI(
        game,
        actionFactory,
        classes,
        combat,
        feats,
        footstepSounds,
        guiSounds,
        objectFactory,
        party,
        portraits,
        reputes,
        scriptRunner,
        soundSets,
        surfaces,
        audioFiles,
        audioPlayer,
        context,
        features,
        fonts,
        lips,
        materials,
        meshes,
        models,
        pbrIbl,
        shaders,
        textures,
        walkmeshes,
        window,
        resources,
        strings),
    _charGen(charGen) {

    _resRef = getResRef("qorcpnl");

    initForGame();
}

void QuickOrCustom::load() {
    GUI::load();
    bindControls();

    if (!_game->isTSL()) {
        _binding.lblRbg->setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f));
    }

    _binding.lbDesc->setProtoMatchContent(true);

    _binding.btnBack->setOnClick([this]() {
        _charGen->openClassSelection();
    });

    _binding.quickCharBtn->setOnFocusChanged([this](bool focus) {
        if (!focus)
            return;
        string text(_strings.get(kStrRefQuickHelpText));
        _binding.lbDesc->clearItems();
        _binding.lbDesc->addTextLinesAsItems(text);
    });
    _binding.quickCharBtn->setOnClick([this]() {
        _charGen->startQuick();
    });

    _binding.custCharBtn->setOnFocusChanged([this](bool focus) {
        if (!focus)
            return;
        string text(_strings.get(kStrRefCustomHelpText));
        _binding.lbDesc->clearItems();
        _binding.lbDesc->addTextLinesAsItems(text);
    });
    _binding.custCharBtn->setOnClick([this]() {
        _charGen->startCustom();
    });
}

void QuickOrCustom::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.custCharBtn = getControl<Button>("CUST_CHAR_BTN");
    _binding.quickCharBtn = getControl<Button>("QUICK_CHAR_BTN");
    _binding.lblRbg = getControl<Label>("LBL_RBG");
    _binding.lbDesc = getControl<ListBox>("LB_DESC");
}

} // namespace game

} // namespace reone
