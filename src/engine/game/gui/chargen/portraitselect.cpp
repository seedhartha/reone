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

#include "portraitselect.h"

#include "../../../common/collectionutil.h"
#include "../../../common/random.h"
#include "../../../gui/scenebuilder.h"
#include "../../../graphics/model/models.h"
#include "../../../graphics/textures.h"
#include "../../../resource/resources.h"
#include "../../../scene/node/modelscenenode.h"

#include "../../game.h"
#include "../../portrait.h"
#include "../../portraits.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 0.2f;

PortraitSelection::PortraitSelection(Game *game, CharacterGeneration *charGen) :
    GameGUI(game->gameId(), game->options().graphics),
    _game(game),
    _charGen(charGen) {

    _resRef = getResRef("portcust");

    initForGame();

    if (_gameId == GameID::KotOR) {
        loadBackground(BackgroundType::Menu);
    }
}

void PortraitSelection::load() {
    GUI::load();

    setButtonColors("BTN_ACCEPT");
    setButtonColors("BTN_BACK");
}

void PortraitSelection::setButtonColors(const string &tag) {
    Control &control = getControl(tag);

    Control::Text text(control.text());
    text.color = getBaseColor(_gameId);
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = getHilightColor(_gameId);
    control.setHilight(move(hilight));
}

void PortraitSelection::loadHeadModel() {
    Control &control = getControl("LBL_HEAD");
    float aspect = control.extent().width / static_cast<float>(control.extent().height);

    unique_ptr<SceneGraph> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&PortraitSelection::getCharacterModel, this, _1))
        .modelScale(kModelScale)
        .cameraFromModelNode(_charGen->character().gender == Gender::Male ? "camerahookm" : "camerahookf")
        .ambientLightColor(glm::vec3(0.2f))
        .build());

    control.setScene(move(scene));
}

shared_ptr<ModelSceneNode> PortraitSelection::getCharacterModel(SceneGraph &sceneGraph) {
    // Create a creature from the current portrait

    auto objectFactory = make_unique<ObjectFactory>(_game, &sceneGraph);

    shared_ptr<Creature> creature(objectFactory->newCreature());
    creature->setFacing(-glm::half_pi<float>());
    creature->setAppearance(getAppearanceFromCurrentPortrait());
    creature->equip("g_a_clothes01");
    creature->loadAppearance();
    creature->getModelSceneNode()->setCullable(false);
    creature->updateModelAnimation();

    // Attach creature model to the root scene node

    shared_ptr<ModelSceneNode> creatureModel(creature->getModelSceneNode());
    glm::vec3 headPosition;
    if (creatureModel->getNodeAbsolutePosition("camerahook", headPosition)) {
        creature->setPosition(glm::vec3(0.0f, 0.0f, -headPosition.z));
    }
    auto model = make_shared<ModelSceneNode>(ModelUsage::GUI, Models::instance().get("cghead_light"), &sceneGraph);
    model->attach("cghead_light", creatureModel);


    return move(model);
}

int PortraitSelection::getAppearanceFromCurrentPortrait() const {
    switch (_charGen->character().attributes.getEffectiveClass()) {
        case ClassType::Scoundrel:
            return _portraits[_currentPortrait].appearanceS;
        case ClassType::Soldier:
            return _portraits[_currentPortrait].appearanceL;
        default:
            return _portraits[_currentPortrait].appearanceNumber;
    }
}

void PortraitSelection::updatePortraits() {
    _portraits.clear();
    int sex = _charGen->character().gender == Gender::Female ? 1 : 0;
    for (auto &portrait : Portraits::instance().portraits()) {
        if (portrait.forPC && portrait.sex == sex) {
            _portraits.push_back(move(portrait));
        }
    }
    resetCurrentPortrait();
}

void PortraitSelection::resetCurrentPortrait() {
    int appearance = _charGen->character().appearance;
    auto maybePortrait = find_if(_portraits.begin(), _portraits.end(), [&appearance](const Portrait &portrait) {
        return
            portrait.appearanceNumber == appearance ||
            portrait.appearanceS == appearance ||
            portrait.appearanceL == appearance;
    });
    if (maybePortrait != _portraits.end()) {
        _currentPortrait = static_cast<int>(distance(_portraits.begin(), maybePortrait));
        loadCurrentPortrait();
        loadHeadModel();
    } else {
        _currentPortrait = -1;
    }
}

void PortraitSelection::loadCurrentPortrait() {
    Control &control = getControl("LBL_PORTRAIT");
    control.setBorderFill(Textures::instance().get(_portraits[_currentPortrait].resRef, TextureUsage::GUI));
}

void PortraitSelection::onClick(const string &control) {
    GameGUI::onClick(control);

    int portraitCount = static_cast<int>(_portraits.size());

    if (control == "BTN_ARRL") {
        _currentPortrait--;
        if (_currentPortrait == -1) {
            _currentPortrait = portraitCount - 1;
        }
        loadCurrentPortrait();
        loadHeadModel();

    } else if (control == "BTN_ARRR") {
        _currentPortrait = (_currentPortrait + 1) % portraitCount;
        loadCurrentPortrait();
        loadHeadModel();

    } else if (control == "BTN_ACCEPT") {
        Character character(_charGen->character());
        character.appearance = getAppearanceFromCurrentPortrait();
        _charGen->setCharacter(move(character));
        _charGen->goToNextStep();
        _charGen->openSteps();

    } else if (control == "BTN_BACK") {
        resetCurrentPortrait();
        _charGen->openSteps();
    }
}

} // namespace game

} // namespace reone
