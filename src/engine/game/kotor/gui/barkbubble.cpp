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

#include "barkbubble.h"

#include "../kotor.h"

using namespace std;

using namespace reone::gui;

namespace reone {

namespace game {

BarkBubble::BarkBubble(
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
        strings) {
    _resRef = getResRef("barkbubble");
    _scaling = ScalingMode::PositionRelativeToCenter;
}

void BarkBubble::load() {
    GUI::load();
    bindControls();

    _rootControl->setVisible(false);
    _binding.lblBarkText->setVisible(false);
}

void BarkBubble::bindControls() {
    _binding.lblBarkText = getControl<Label>("LBL_BARKTEXT");
}

void BarkBubble::update(float dt) {
    if (_timer.advance(dt)) {
        setBarkText("", 0.0f);
    }
}

void BarkBubble::setBarkText(const string &text, float duration) {
    if (text.empty()) {
        _rootControl->setVisible(false);
        _binding.lblBarkText->setVisible(false);
    } else {
        float textWidth = _binding.lblBarkText->text().font->measure(text);
        int lineCount = static_cast<int>(textWidth / static_cast<float>(_binding.lblBarkText->extent().width)) + 1;
        int padding = _binding.lblBarkText->extent().left;
        float rootHeight = lineCount * _binding.lblBarkText->text().font->height() + 2 * padding;
        float labelHeight = lineCount * _binding.lblBarkText->text().font->height();

        _rootControl->setVisible(true);
        _rootControl->setExtentHeight(static_cast<int>(rootHeight));

        _binding.lblBarkText->setExtentHeight(static_cast<int>(labelHeight));
        _binding.lblBarkText->setTextMessage(text);
        _binding.lblBarkText->setVisible(true);
    }

    if (duration > 0.0f) {
        _timer.setTimeout(duration);
    }
}

} // namespace game

} // namespace reone
