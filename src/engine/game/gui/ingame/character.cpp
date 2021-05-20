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

#include "character.h"

#include <boost/format.hpp>

#include "../../../gui/scenebuilder.h"
#include "../../../graphics/model/models.h"

#include "../../d20/classes.h"
#include "../../game.h"

#include "../colorutil.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

CharacterMenu::CharacterMenu(Game *game) : GameGUI(game) {
    _resRef = getResRef("character");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void CharacterMenu::load() {
    GUI::load();

    hideControl("BTN_CHARLEFT");
    hideControl("BTN_CHARRIGHT");
    hideControl("LBL_GOOD0");
    hideControl("LBL_GOOD1");
    hideControl("LBL_GOOD2");
    hideControl("LBL_GOOD3");
    hideControl("LBL_GOOD4");
    hideControl("LBL_GOOD5");
    hideControl("LBL_GOOD6");
    hideControl("LBL_GOOD7");
    hideControl("LBL_GOOD8");
    hideControl("LBL_GOOD9");
    hideControl("LBL_GOOD10");
    hideControl("LBL_MORE");

    disableControl("BTN_AUTO");
    disableControl("BTN_SCRIPTS");
}

void CharacterMenu::update(float dt) {
    shared_ptr<Creature> leader(_game->party().getLeader());
    setControlVisible("BTN_LEVELUP", leader->isLevelUpPending());
    setControlVisible("BTN_AUTO", leader->isLevelUpPending());
    GUI::update(dt);
}

static string toStringOrEmptyIfZero(int value) {
    return value != 0 ? to_string(value) : "";
}

static string describeAbilityModifier(int value) {
    return value > 0 ? "+" + to_string(value) : to_string(value);
}

void CharacterMenu::refreshControls() {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    CreatureAttributes &attributes = partyLeader->attributes();

    setControlText("LBL_CLASS1", describeClass(attributes.getClassByPosition(1)));
    setControlText("LBL_CLASS2", describeClass(attributes.getClassByPosition(2)));
    setControlText("LBL_LEVEL1", toStringOrEmptyIfZero(attributes.getLevelByPosition(1)));
    setControlText("LBL_LEVEL2", toStringOrEmptyIfZero(attributes.getLevelByPosition(2)));

    setControlText("LBL_VITALITY_STAT", str(boost::format("%d/%d") % partyLeader->currentHitPoints() % partyLeader->hitPoints()));
    setControlText("LBL_DEFENSE_STAT", to_string(attributes.getDefense()));
    setControlText("LBL_FORCE_STAT", "");

    setControlText("LBL_STR", to_string(attributes.strength()));
    setControlText("LBL_STR_MOD", describeAbilityModifier(attributes.getAbilityModifier(Ability::Strength)));
    setControlText("LBL_DEX", to_string(attributes.dexterity()));
    setControlText("LBL_DEX_MOD", describeAbilityModifier(attributes.getAbilityModifier(Ability::Dexterity)));
    setControlText("LBL_CON", to_string(attributes.constitution()));
    setControlText("LBL_CON_MOD", describeAbilityModifier(attributes.getAbilityModifier(Ability::Constitution)));
    setControlText("LBL_INT", to_string(attributes.intelligence()));
    setControlText("LBL_INT_MOD", describeAbilityModifier(attributes.getAbilityModifier(Ability::Intelligence)));
    setControlText("LBL_WIS", to_string(attributes.wisdom()));
    setControlText("LBL_WIS_MOD", describeAbilityModifier(attributes.getAbilityModifier(Ability::Wisdom)));
    setControlText("LBL_CHA", to_string(attributes.charisma()));
    setControlText("LBL_CHA_MOD", describeAbilityModifier(attributes.getAbilityModifier(Ability::Charisma)));

    SavingThrows savingThrows(attributes.getAggregateSavingThrows());
    setControlText("LBL_FORTITUDE_STAT", to_string(savingThrows.fortitude));
    setControlText("LBL_REFLEX_STAT", to_string(savingThrows.reflex));
    setControlText("LBL_WILL_STAT", to_string(savingThrows.will));

    setControlText("LBL_EXPERIENCE_STAT", to_string(partyLeader->xp()));
    setControlText("LBL_NEEDED_XP", to_string(partyLeader->getNeededXP()));

    refreshPortraits();
    refresh3D();
}

string CharacterMenu::describeClass(ClassType clazz) const {
    if (clazz == ClassType::Invalid) return "";

    return _game->classes().get(clazz)->name();
}

void CharacterMenu::refreshPortraits() {
    if (_game->gameId() != GameID::KotOR) return;

    Party &party = _game->party();
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    Control &btnChange1 = getControl("BTN_CHANGE1");
    btnChange1.setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    btnChange1.setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    Control &btnChange2 = getControl("BTN_CHANGE2");
    btnChange2.setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    btnChange2.setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::refresh3D() {
    Control &control3d = getControl("LBL_3DCHAR");
    float aspect = control3d.extent().width / static_cast<float>(control3d.extent().height);

    auto scene = SceneBuilder(_gfxOpts, _shaders, _meshes, _textures, &_game->materials(), &_game->pbrIbl())
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&CharacterMenu::getSceneModel, this, _1))
        .modelOffset(glm::vec2(0.0f, 1.7f))
        .cameraFromModelNode("camerahook")
        .ambientLightColor(glm::vec3(0.2f))
        .build();

    control3d.setScene(move(scene));
}

shared_ptr<ModelSceneNode> CharacterMenu::getSceneModel(SceneGraph &sceneGraph) const {
    auto partyLeader = _game->party().getLeader();
    auto objectFactory = make_shared<ObjectFactory>(_game, &sceneGraph);
    auto character = objectFactory->newCreature();
    character->setFacing(-glm::half_pi<float>());
    character->setAppearance(partyLeader->appearance());
    for (auto &item : partyLeader->equipment()) {
        switch (item.first) {
            case InventorySlot::body:
                character->equip(item.first, item.second);
                break;
            default:
                break;
        }
    }
    character->loadAppearance();
    character->updateModelAnimation();

    auto sceneModel = make_shared<ModelSceneNode>(_game->models().get("charmain_light"), ModelUsage::GUI, &sceneGraph);
    sceneModel->attach("charmain_light", character->sceneNode());

    return move(sceneModel);
}

void CharacterMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    } else if (control == "BTN_LEVELUP") {
        _game->openLevelUp();
    }
}

} // namespace game

} // namespace reone
