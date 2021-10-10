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

#include "container.h"

#include "../../../graphics/texture/textures.h"
#include "../../../gui/control/imagebutton.h"
#include "../../../resource/strings.h"

#include "../../core/object/creature.h"
#include "../../core/object/item.h"
#include "../../core/object/placeable.h"
#include "../../core/party.h"

#include "../kotor.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kSwitchToResRef = 47884;
static constexpr int kGiveItemResRef = 47885;
static constexpr int kInventoryResRef = 393;

ContainerGUI::ContainerGUI(
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
    _resRef = getResRef("container");

    initForGame();
}

void ContainerGUI::load() {
    GUI::load();
    bindControls();

    string btnMessage(_strings.get(kSwitchToResRef) + " " + _strings.get(kGiveItemResRef));
    _binding.btnGiveItems->setTextMessage(btnMessage);

    string lblMessage(_strings.get(kInventoryResRef));
    _binding.lblMessage->setTextMessage(lblMessage);

    _binding.btnOk->setOnClick([this]() {
        transferItemsToPlayer();
        _game->openInGame();
    });
    _binding.btnCancel->setOnClick([this]() {
        _game->openInGame();
    });

    configureItemsListBox();
}

void ContainerGUI::bindControls() {
    _binding.lblMessage = getControl<Label>("LBL_MESSAGE");
    _binding.lbItems = getControl<ListBox>("LB_ITEMS");
    _binding.btnOk = getControl<Button>("BTN_OK");
    _binding.btnGiveItems = getControl<Button>("BTN_GIVEITEMS");
    _binding.btnCancel = getControl<Button>("BTN_CANCEL");
}

void ContainerGUI::configureItemsListBox() {
    ImageButton &protoItem = static_cast<ImageButton &>(_binding.lbItems->protoItem());

    Control::Text text(protoItem.text());
    text.align = Control::TextAlign::LeftTop;

    protoItem.setText(text);
}

void ContainerGUI::open(shared_ptr<SpatialObject> container) {
    _binding.lbItems->clearItems();

    for (auto &item : container->items()) {
        if (!item->isDropable())
            continue;

        ListBox::Item lbItem;
        lbItem.tag = item->tag();
        lbItem.text = item->localizedName();
        lbItem.iconTexture = item->icon();
        lbItem.iconFrame = getItemFrameTexture(item->stackSize());

        if (item->stackSize() > 1) {
            lbItem.iconText = to_string(item->stackSize());
        }
        _binding.lbItems->addItem(move(lbItem));
    }

    _container = move(container);
}

shared_ptr<Texture> ContainerGUI::getItemFrameTexture(int stackSize) const {
    string resRef;
    if (_game->isTSL()) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return _textures.get(resRef, TextureUsage::GUI);
}

void ContainerGUI::transferItemsToPlayer() {
    shared_ptr<Creature> player(_party.player());
    _container->moveDropableItemsTo(*player);

    auto placeable = dynamic_pointer_cast<Placeable>(_container);
    if (placeable) {
        placeable->runOnInvDisturbed(player);
    }
}

} // namespace game

} // namespace reone
