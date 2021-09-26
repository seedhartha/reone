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
#include "../../../common/randomutil.h"
#include "../../../graphics/model/models.h"
#include "../../../graphics/texture/textures.h"
#include "../../../gui/scenebuilder.h"
#include "../../../resource/resources.h"
#include "../../../scene/node/model.h"

#include "../../game.h"
#include "../../object/factory.h"
#include "../../portrait.h"
#include "../../portraits.h"

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

PortraitSelection::PortraitSelection(CharacterGeneration *charGen, Game *game) :
    GameGUI(game),
    _charGen(charGen) {

    _resRef = getResRef("portcust");

    initForGame();

    if (game->isKotOR()) {
        loadBackground(BackgroundType::Menu);
    }
}

void PortraitSelection::load() {
    GUI::load();
    bindControls();

    setButtonColors(*_binding.btnAccept);
    setButtonColors(*_binding.btnBack);

    _binding.btnArrL->setOnClick([this]() {
        _currentPortrait--;
        if (_currentPortrait == -1) {
            _currentPortrait = static_cast<int>(_portraits.size()) - 1;
        }
        loadCurrentPortrait();
        loadHeadModel();
    });
    _binding.btnArrR->setOnClick([this]() {
        _currentPortrait = (_currentPortrait + 1) % static_cast<int>(_portraits.size());
        loadCurrentPortrait();
        loadHeadModel();
    });
    _binding.btnAccept->setOnClick([this]() {
        Character character(_charGen->character());
        character.appearance = getAppearanceFromCurrentPortrait();
        _charGen->setCharacter(move(character));
        _charGen->goToNextStep();
        _charGen->openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        resetCurrentPortrait();
        _charGen->openSteps();
    });
}

void PortraitSelection::bindControls() {
    _binding.lblHead = getControl<Label>("LBL_HEAD");
    _binding.lblPortrait = getControl<Label>("LBL_PORTRAIT");
    _binding.btnArrL = getControl<Button>("BTN_ARRL");
    _binding.btnArrR = getControl<Button>("BTN_ARRR");
    _binding.btnAccept = getControl<Button>("BTN_ACCEPT");
    _binding.btnBack = getControl<Button>("BTN_BACK");
}

void PortraitSelection::setButtonColors(Control &control) {
    Control::Text text(control.text());
    text.color = _game->getGUIColorBase();
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = _game->getGUIColorHilight();
    control.setHilight(move(hilight));
}

void PortraitSelection::loadHeadModel() {
    float aspect = _binding.lblHead->extent().width / static_cast<float>(_binding.lblHead->extent().height);

    unique_ptr<SceneGraph> scene(SceneBuilder(
                                     _options,
                                     _context,
                                     _features,
                                     _materials,
                                     _meshes,
                                     _pbrIbl,
                                     _shaders,
                                     _textures)
                                     .aspect(aspect)
                                     .depth(0.1f, 10.0f)
                                     .modelSupplier(bind(&PortraitSelection::getCharacterModel, this, _1))
                                     .modelScale(kModelScale)
                                     .cameraFromModelNode(_charGen->character().gender == Gender::Male ? "camerahookm" : "camerahookf")
                                     .lightingRefFromModelNode("cghead_light")
                                     .build());

    _binding.lblHead->setScene(move(scene));
}

shared_ptr<ModelSceneNode> PortraitSelection::getCharacterModel(SceneGraph &sceneGraph) {
    // Create a creature from the current portrait

    auto objectFactory = make_unique<ObjectFactory>(sceneGraph);
    objectFactory->setGame(*_game);

    shared_ptr<Creature> creature(objectFactory->newCreature());
    creature->setFacing(-glm::half_pi<float>());
    creature->setAppearance(getAppearanceFromCurrentPortrait());
    creature->equip("g_a_clothes01");
    creature->loadAppearance();
    creature->sceneNode()->setCullable(false);
    creature->updateModelAnimation();

    // Attach creature model to the root scene node

    auto creatureModel = static_pointer_cast<ModelSceneNode>(creature->sceneNode());
    shared_ptr<ModelNode> cameraHook(creatureModel->model()->getNodeByName("camerahook"));
    if (cameraHook) {
        creature->setPosition(glm::vec3(0.0f, 0.0f, -cameraHook->absoluteTransform()[3].z));
    }
    auto model = sceneGraph.newModel(_game->models().get("cghead_light"), ModelUsage::GUI);
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
    for (auto &portrait : _game->portraits().portraits()) {
        if (portrait.forPC && portrait.sex == sex) {
            _portraits.push_back(move(portrait));
        }
    }
    resetCurrentPortrait();
}

void PortraitSelection::resetCurrentPortrait() {
    int appearance = _charGen->character().appearance;
    auto maybePortrait = find_if(_portraits.begin(), _portraits.end(), [&appearance](const Portrait &portrait) {
        return portrait.appearanceNumber == appearance ||
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
    string resRef(_portraits[_currentPortrait].resRef);
    shared_ptr<Texture> portrait(_game->textures().get(resRef, TextureUsage::GUI));
    _binding.lblPortrait->setBorderFill(portrait);
}

} // namespace game

} // namespace reone
