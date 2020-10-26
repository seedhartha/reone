/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "chargen.h"

#include <stdexcept>

#include "../../../gui/scenebuilder.h"
#include "../../../resource/resources.h"

#include "../../game.h"
#include "../../portraits.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static const float kModelScale = 1.05f;
static const float kModelOffsetY = 0.9f;

CharacterGeneration::CharacterGeneration(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    if (!game) {
        throw invalid_argument("Game must not be null");
    }
    _resRef = getResRef("maincg");
    _backgroundType = BackgroundType::Menu;

    if (game->version() == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
};

void CharacterGeneration::load() {
    GUI::load();

    hideControl("VIT_ARROW_LBL");
    hideControl("DEF_ARROW_LBL");
    hideControl("FORT_ARROW_LBL");
    hideControl("REFL_ARROW_LBL");
    hideControl("WILL_ARROW_LBL");
    hideControl("OLD_LBL");
    hideControl("NEW_LBL");

    setControlText("LBL_NAME", "");
    setControlText("LBL_CLASS", "");
    setControlText("LBL_LEVEL", "");
    setControlText("LBL_LEVEL_VAL", "");

    loadClassSelection();
    loadQuickOrCustom();
    loadQuick();
    loadPortraitSelection();
    loadNameEntry();
}

void CharacterGeneration::loadClassSelection() {
    _classSelection = make_unique<ClassSelection>(_game);
    _classSelection->load();
}

void CharacterGeneration::loadQuickOrCustom() {
    _quickOrCustom = make_unique<QuickOrCustom>(this, _version, _gfxOpts);
    _quickOrCustom->load();
}

void CharacterGeneration::loadQuick() {
    _quick = make_unique<QuickCharacterGeneration>(this, _version, _gfxOpts);
    _quick->load();
}

void CharacterGeneration::loadPortraitSelection() {
    _portraitSelection = make_unique<PortraitSelection>(this, _version, _gfxOpts);
    _portraitSelection->load();
}

void CharacterGeneration::loadNameEntry() {
    _nameEntry = make_unique<NameEntry>(this, _version, _gfxOpts);
    _nameEntry->load();
}

bool CharacterGeneration::handle(const SDL_Event &event) {
    if (getSubGUI()->handle(event)) {
        return true;
    }
    return GUI::handle(event);
}

GUI *CharacterGeneration::getSubGUI() const {
    switch (_screen) {
        case CharGenScreen::ClassSelection:
            return _classSelection.get();
        case CharGenScreen::QuickOrCustom:
            return _quickOrCustom.get();
        case CharGenScreen::Quick:
            return _quick.get();
        case CharGenScreen::PortraitSelection:
            return _portraitSelection.get();
        case CharGenScreen::Name:
            return _nameEntry.get();
        default:
            throw logic_error("CharGen: invalid screen: " + to_string(static_cast<int>(_screen)));
    }
}

void CharacterGeneration::update(float dt) {
    GUI::update(dt);
    getSubGUI()->update(dt);
}

void CharacterGeneration::render() const {
    GUI::render();
    getSubGUI()->render();
}

void CharacterGeneration::render3D() const {
    GUI::render3D();
    getSubGUI()->render3D();
}

void CharacterGeneration::finish() {
    string moduleName(_version == GameVersion::KotOR ? "end_m01aa" : "001ebo");

    CreatureConfiguration config(_character);
    config.equipment.clear();

    PartyConfiguration party;
    party.memberCount = 1;
    party.leader = config;

    _game->loadModule(moduleName, party);
}

void CharacterGeneration::cancel() {
    _game->openMainMenu();
}

void CharacterGeneration::openClassSelection() {
    hideControl("MODEL_LBL");
    _screen = CharGenScreen::ClassSelection;
}

void CharacterGeneration::openNameEntry() {
    hideControl("MODEL_LBL");
    _screen = CharGenScreen::Name;
}

void CharacterGeneration::openPortraitSelection() {
    hideControl("MODEL_LBL");
    _screen = CharGenScreen::PortraitSelection;
}

void CharacterGeneration::openQuick() {
    showControl("MODEL_LBL");
    _screen = CharGenScreen::Quick;
}

void CharacterGeneration::openQuickOrCustom() {
    showControl("MODEL_LBL");
    _screen = CharGenScreen::QuickOrCustom;
}

void CharacterGeneration::loadCharacterModel() {
    Control &lblModel = getControl("MODEL_LBL");
    const Control::Extent &extent = lblModel.extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    glm::mat4 cameraTransform(1.0f);
    cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 1.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));

    unique_ptr<Control::Scene3D> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&CharacterGeneration::getCharacterModel, this, _1))
        .modelScale(kModelScale)
        .modelOffset(glm::vec2(0.0f, kModelOffsetY))
        .cameraTransform(cameraTransform)
        .ambientLightColor(glm::vec3(1.0f))
        .build());

    lblModel.setScene3D(move(scene));

    string portrait(findPortrait(_character.appearance));
    if (!portrait.empty()) {
        Control &lblPortrait = getControl("PORTRAIT_LBL");
        lblPortrait.setBorderFill(portrait);
    }
}

shared_ptr<ModelSceneNode> CharacterGeneration::getCharacterModel(SceneGraph &sceneGraph) {
    unique_ptr<ObjectFactory> objectFactory(new ObjectFactory(_game, &sceneGraph));

    unique_ptr<Creature> creature(objectFactory->newCreature());
    creature->load(_character);

    return creature->model();
}

const CreatureConfiguration &CharacterGeneration::character() const {
    return _character;
}

void CharacterGeneration::setCharacter(const CreatureConfiguration &config) {
    _character = config;
    loadCharacterModel();
    _portraitSelection->updatePortraits();
}

void CharacterGeneration::setQuickStep(int step) {
    _quick->setStep(step);
}

} // namespace game

} // namespace reone
