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

#include "inventory.h"

#include "../../../../gui/control/button.h"
#include "../../../../gui/control/label.h"
#include "../../../../gui/control/listbox.h"

#include "../../../core/object/creature.h"
#include "../../../core/party.h"

#include "../../kotor.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

InventoryMenu::InventoryMenu(
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
    AudioFiles &audioFiles,
    AudioPlayer &audioPlayer,
    Context &context,
    Features &features,
    Fonts &fonts,
    Lips &lips,
    Materials &materials,
    Meshes &meshes,
    Models &models,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    Walkmeshes &walkmeshes,
    Window &window,
    Resources &resources,
    Strings &strings,
    TwoDas &twoDas) :
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
        strings,
        twoDas) {
    _resRef = getResRef("inventory");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void InventoryMenu::load() {
    GUI::load();
    bindControls();

    _binding.lblCreditsValue->setVisible(false);
    _binding.btnUseItem->setDisabled(true);
    _binding.btnExit->setOnClick([this]() {
        _game->openInGame();
    });

    if (!_game->isTSL()) {
        _binding.lblVit->setVisible(false);
        _binding.lblDef->setVisible(false);
        _binding.btnChange1->setFocusable(false);
        _binding.btnChange2->setFocusable(false);
        _binding.btnQuestItems->setDisabled(true);
    }
}

void InventoryMenu::bindControls() {
    _binding.btnExit = getControl<Button>("BTN_EXIT");
    _binding.btnUseItem = getControl<Button>("BTN_USEITEM");
    _binding.lblCredits = getControl<Label>("LBL_CREDITS");
    _binding.lblCreditsValue = getControl<Label>("LBL_CREDITS_VALUE");
    _binding.lblInv = getControl<Label>("LBL_INV");
    _binding.lbDescription = getControl<ListBox>("LB_DESCRIPTION");
    _binding.lbItems = getControl<ListBox>("LB_ITEMS");

    if (!_game->isTSL()) {
        _binding.btnChange1 = getControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControl<Button>("BTN_CHANGE2");
        _binding.btnQuestItems = getControl<Button>("BTN_QUESTITEMS");
        _binding.lblBgPort = getControl<Label>("LBL_BGPORT");
        _binding.lblBgStats = getControl<Label>("LBL_BGSTATS");
        _binding.lblDef = getControl<Label>("LBL_DEF");
        _binding.lblPort = getControl<Label>("LBL_PORT");
        _binding.lblVit = getControl<Label>("LBL_VIT");
    } else {
        _binding.btnAll = getControl<Button>("BTN_ALL");
        _binding.btnArmor = getControl<Button>("BTN_ARMOR");
        _binding.btnDatapads = getControl<Button>("BTN_DATAPADS");
        _binding.btnMisc = getControl<Button>("BTN_MISC");
        _binding.btnQuests = getControl<Button>("BTN_QUESTS");
        _binding.btnUseable = getControl<Button>("BTN_USEABLE");
        _binding.btnWeapons = getControl<Button>("BTN_WEAPONS");
        _binding.lblBar1 = getControl<Label>("LBL_BAR1");
        _binding.lblBar2 = getControl<Label>("LBL_BAR2");
        _binding.lblBar3 = getControl<Label>("LBL_BAR3");
        _binding.lblBar4 = getControl<Label>("LBL_BAR4");
        _binding.lblBar5 = getControl<Label>("LBL_BAR5");
        _binding.lblBar6 = getControl<Label>("LBL_BAR6");
        _binding.lblFilter = getControl<Label>("LBL_FILTER");
    }
}

void InventoryMenu::refreshPortraits() {
    if (!!_game->isTSL())
        return;

    Party &party = _party;
    shared_ptr<Creature> partyLeader(party.getLeader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.lblPort->setBorderFill(partyLeader->portrait());

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

} // namespace game

} // namespace reone
