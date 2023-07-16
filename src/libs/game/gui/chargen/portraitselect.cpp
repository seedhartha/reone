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

#include "reone/game/gui/chargen/portraitselect.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/game/object/factory.h"
#include "reone/game/portrait.h"
#include "reone/game/portraits.h"
#include "reone/game/types.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/models.h"
#include "reone/graphics/textures.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/resource/resources.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/model.h"
#include "reone/system/randomutil.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 0.2f;

void PortraitSelection::onGUILoaded() {
    if (!_game.isTSL()) {
        loadBackground(BackgroundType::Menu);
    }

    bindControls();

    setButtonColors(*_controls.BTN_ACCEPT);
    setButtonColors(*_controls.BTN_BACK);

    _controls.BTN_ARRL->setOnClick([this]() {
        _currentPortrait--;
        if (_currentPortrait == -1) {
            _currentPortrait = static_cast<int>(_filteredPortraits.size()) - 1;
        }
        loadCurrentPortrait();
        loadHeadModel();
    });
    _controls.BTN_ARRR->setOnClick([this]() {
        _currentPortrait = (_currentPortrait + 1) % static_cast<int>(_filteredPortraits.size());
        loadCurrentPortrait();
        loadHeadModel();
    });
    _controls.BTN_ACCEPT->setOnClick([this]() {
        Character character(_charGen.character());
        character.appearance = getAppearanceFromCurrentPortrait();
        _charGen.setCharacter(std::move(character));
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _controls.BTN_BACK->setOnClick([this]() {
        resetCurrentPortrait();
        _charGen.openSteps();
    });
}

void PortraitSelection::setButtonColors(Control &control) {
    Control::Text text(control.text());
    text.color = _baseColor;
    control.setText(std::move(text));

    Control::Border hilight(control.hilight());
    hilight.color = _hilightColor;
    control.setHilight(std::move(hilight));
}

void PortraitSelection::loadHeadModel() {
    auto &sceneGraph = _services.scene.graphs.get(kScenePortraitSelect);
    float aspect = _controls.LBL_HEAD->extent().width / static_cast<float>(_controls.LBL_HEAD->extent().height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&PortraitSelection::getCharacterModel, this, std::placeholders::_1))
        .modelScale(kModelScale)
        .cameraFromModelNode(_charGen.character().gender == Gender::Male ? "camerahookm" : "camerahookf")
        .invoke();

    _controls.LBL_HEAD->setSceneName(kScenePortraitSelect);
}

std::shared_ptr<ModelSceneNode> PortraitSelection::getCharacterModel(ISceneGraph &sceneGraph) {
    // Create a creature from the current portrait

    std::shared_ptr<Creature> creature = _game.objectFactory().newCreature(sceneGraph.name());
    _game.addObject(creature);
    creature->setFacing(-glm::half_pi<float>());
    creature->setAppearance(getAppearanceFromCurrentPortrait());
    creature->equip("g_a_clothes01");
    creature->loadAppearance();
    creature->sceneNode()->setCullable(false);
    creature->updateModelAnimation();

    // Attach creature model to the root scene node

    auto creatureModel = std::static_pointer_cast<ModelSceneNode>(creature->sceneNode());
    auto cameraHook = creatureModel->getNodeByName("camerahook");
    if (cameraHook) {
        creature->setPosition(glm::vec3(0.0f, 0.0f, -cameraHook->getOrigin().z));
    }
    auto model = sceneGraph.newModel(*_services.graphics.models.get("cghead_light"), ModelUsage::GUI);
    model->attach("cghead_light", *creatureModel);

    return model;
}

int PortraitSelection::getAppearanceFromCurrentPortrait() const {
    switch (_charGen.character().attributes.getEffectiveClass()) {
    case ClassType::Scoundrel:
        return _filteredPortraits[_currentPortrait].appearanceS;
    case ClassType::Soldier:
        return _filteredPortraits[_currentPortrait].appearanceL;
    default:
        return _filteredPortraits[_currentPortrait].appearanceNumber;
    }
}

void PortraitSelection::updatePortraits() {
    _filteredPortraits.clear();
    int sex = _charGen.character().gender == Gender::Female ? 1 : 0;
    for (auto &portrait : _services.game.portraits.portraits()) {
        if (portrait.forPC && portrait.sex == sex) {
            _filteredPortraits.push_back(std::move(portrait));
        }
    }
    resetCurrentPortrait();
}

void PortraitSelection::resetCurrentPortrait() {
    int appearance = _charGen.character().appearance;
    auto maybePortrait = std::find_if(_filteredPortraits.begin(), _filteredPortraits.end(), [&appearance](const Portrait &portrait) {
        return portrait.appearanceNumber == appearance ||
               portrait.appearanceS == appearance ||
               portrait.appearanceL == appearance;
    });
    if (maybePortrait != _filteredPortraits.end()) {
        _currentPortrait = static_cast<int>(distance(_filteredPortraits.begin(), maybePortrait));
        loadCurrentPortrait();
        loadHeadModel();
    } else {
        _currentPortrait = -1;
    }
}

void PortraitSelection::loadCurrentPortrait() {
    std::string resRef(_filteredPortraits[_currentPortrait].resRef);
    std::shared_ptr<Texture> portrait(_services.graphics.textures.get(resRef, TextureUsage::GUI));
    _controls.LBL_PORTRAIT->setBorderFill(portrait);
}

} // namespace game

} // namespace reone
