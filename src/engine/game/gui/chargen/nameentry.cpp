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

#include "nameentry.h"

#include "../../../common/streamutil.h"
#include "../../../resource/resources.h"

#include "../../game.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

NameEntry::NameEntry(
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
    _charGen(charGen),
    _input(TextInputFlags::lettersWhitespace) {

    _resRef = getResRef("name");

    initForGame();
}

void NameEntry::load() {
    GUI::load();
    bindControls();

    loadLtrFile("humanm", _maleLtr);
    loadLtrFile("humanf", _femaleLtr);
    loadLtrFile("humanl", _lastNameLtr);

    _binding.nameBoxEdit->setTextMessage("");

    _binding.btnRandom->setOnClick([this]() {
        loadRandomName();
    });
    _binding.endBtn->setOnClick([this]() {
        _charGen->goToNextStep();
        _charGen->openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen->openSteps();
    });
}

void NameEntry::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnRandom = getControl<Button>("BTN_RANDOM");
    _binding.endBtn = getControl<Button>("END_BTN");
    _binding.nameBoxEdit = getControl<Control>("NAME_BOX_EDIT");
}

void NameEntry::loadLtrFile(const string &resRef, LtrReader &ltr) {
    shared_ptr<ByteArray> data(_resources.getRaw(resRef, ResourceType::Ltr));
    ltr.load(wrap(data));
}

bool NameEntry::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && _input.handle(event)) {
        _binding.nameBoxEdit->setTextMessage(_input.text());
        return true;
    }
    return GUI::handle(event);
}

void NameEntry::loadRandomName() {
    _binding.nameBoxEdit->setTextMessage(getRandomName());
}

string NameEntry::getRandomName() const {
    Gender gender = _charGen->character().gender;
    const LtrReader &nameLtr = gender == Gender::Female ? _femaleLtr : _maleLtr;
    return nameLtr.getRandomName(8) + " " + _lastNameLtr.getRandomName(8);
}

} // namespace game

} // namespace reone
