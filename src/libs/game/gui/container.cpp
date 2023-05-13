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

#include "reone/game/gui/container.h"

#include "reone/graphics/textures.h"
#include "reone/gui/control/imagebutton.h"
#include "reone/resource/strings.h"

#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/item.h"
#include "reone/game/object/placeable.h"
#include "reone/game/party.h"
#include "reone/game/di/services.h"

using namespace std;

using namespace reone::audio;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kSwitchToResRef = 47884;
static constexpr int kGiveItemResRef = 47885;
static constexpr int kInventoryResRef = 393;

ContainerGUI::ContainerGUI(Game &game, ServicesView &services) :
    GameGUI(game, services) {
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
        _game.openInGame();
    });
    _binding.btnCancel->setOnClick([this]() {
        _game.openInGame();
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

void ContainerGUI::open(shared_ptr<Object> container) {
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
    if (_game.isTSL()) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return _textures.get(resRef, TextureUsage::GUI);
}

void ContainerGUI::transferItemsToPlayer() {
    shared_ptr<Creature> player(_game.party().player());
    _container->moveDropableItemsTo(*player);

    auto placeable = dynamic_pointer_cast<Placeable>(_container);
    if (placeable) {
        placeable->runOnInvDisturbed(player);
    }
}

} // namespace game

} // namespace reone
