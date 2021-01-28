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

#include "../../../common/random.h"
#include "../../../gui/scenebuilder.h"
#include "../../../render/models.h"
#include "../../../render/textures.h"
#include "../../../resource/resources.h"
#include "../../../scene/node/modelscenenode.h"

#include "../../blueprint/creature.h"
#include "../../game.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 0.2f;

PortraitSelection::PortraitSelection(Game *game, CharacterGeneration *charGen) :
    GameGUI(game->version(), game->options().graphics),
    _game(game),
    _charGen(charGen) {

    _resRef = getResRef("portcust");

    if (_version == GameVersion::KotOR) {
        _backgroundType = BackgroundType::Menu;
    }

    initForGame();
}

void PortraitSelection::load() {
    GUI::load();

    setButtonColors("BTN_ACCEPT");
    setButtonColors("BTN_BACK");
}

void PortraitSelection::setButtonColors(const string &tag) {
    Control &control = getControl(tag);

    Control::Text text(control.text());
    text.color = getBaseColor(_version);
    control.setText(move(text));

    Control::Border hilight(control.hilight());
    hilight.color = getHilightColor(_version);
    control.setHilight(move(hilight));
}

void PortraitSelection::loadHeadModel() {
    Control &control = getControl("LBL_HEAD");
    float aspect = control.extent().width / static_cast<float>(control.extent().height);

    unique_ptr<Control::Scene3D> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&PortraitSelection::getCharacterModel, this, _1))
        .modelScale(kModelScale)
        .cameraFromModelNode(_charGen->character().gender() == Gender::Male ? "camerahookm" : "camerahookf")
        .ambientLightColor(glm::vec3(0.2f))
        .build());

    control.setScene3D(move(scene));
}

shared_ptr<ModelSceneNode> PortraitSelection::getCharacterModel(SceneGraph &sceneGraph) {
    auto root = make_shared<ModelSceneNode>(&sceneGraph, Models::instance().get("cghead_light"));

    // Create a creature from the current portrait

    auto objectFactory = make_unique<ObjectFactory>(_game, &sceneGraph);
    unique_ptr<Creature> creature(objectFactory->newCreature());

    auto character = make_shared<StaticCreatureBlueprint>(_charGen->character());
    character->setAppearance(getAppearanceFromCurrentPortrait());
    creature->load(character);
    creature->setFacing(-glm::half_pi<float>());

    // Attach creature model to the root scene node

    shared_ptr<ModelSceneNode> creatureModel(creature->model());
    glm::vec3 headPosition;
    if (creatureModel->getNodeAbsolutePosition("camerahook", headPosition)) {
        creature->setPosition(glm::vec3(0.0f, 0.0f, -headPosition.z));
    }
    creature->updateModelAnimation();
    root->attach("cghead_light", creatureModel);


    return move(root);
}

int PortraitSelection::getAppearanceFromCurrentPortrait() const {
    switch (_charGen->character().attributes().getEffectiveClass()) {
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

    shared_ptr<TwoDaTable> portraits(Resources::instance().get2DA("portraits"));
    const StaticCreatureBlueprint &character = _charGen->character();
    int sex = character.gender() == Gender::Female ? 1 : 0;

    for (auto &row : portraits->rows()) {
        if (row.getInt("forpc") == 1 && row.getInt("sex") == sex) {
            string resRef(row.getString("baseresref"));
            int appearanceNumber = row.getInt("appearancenumber");
            int appearanceS = row.getInt("appearance_s");
            int appearanceL = row.getInt("appearance_l");

            shared_ptr<Texture> image(Textures::instance().get(resRef, TextureType::GUI));

            Portrait portrait;
            portrait.resRef = move(resRef);
            portrait.image = move(image);
            portrait.appearanceNumber = appearanceNumber;
            portrait.appearanceS = appearanceS;
            portrait.appearanceL = appearanceL;

            _portraits.push_back(move(portrait));
        }
    }

    resetCurrentPortrait();
}

void PortraitSelection::resetCurrentPortrait() {
    const StaticCreatureBlueprint &character = _charGen->character();
    auto maybePortrait = find_if(_portraits.begin(), _portraits.end(), [&character](const Portrait &portrait) {
        return
            portrait.appearanceNumber == character.appearance() ||
            portrait.appearanceS == character.appearance() ||
            portrait.appearanceL == character.appearance();
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

    Control::Border border(control.border());
    border.fill = _portraits[_currentPortrait].image;

    control.setBorder(move(border));
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
        StaticCreatureBlueprint character(_charGen->character());
        character.setAppearance(getAppearanceFromCurrentPortrait());
        _charGen->setCharacter(character);
        _charGen->goToNextStep();
        _charGen->openSteps();

    } else if (control == "BTN_BACK") {
        resetCurrentPortrait();
        _charGen->openSteps();
    }
}

} // namespace game

} // namespace reone
