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

#include "../../graphics/texture/textures.h"
#include "../../gui/control/imagebutton.h"
#include "../../resource/strings.h"

#include "../game.h"
#include "../gameidutil.h"
#include "../object/item.h"
#include "../objectconverter.h"

#include "colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kSwitchToResRef = 47884;
static constexpr int kGiveItemResRef = 47885;
static constexpr int kInventoryResRef = 393;

Container::Container(Game *game) : GameGUI(game) {
    _resRef = getResRef("container");

    initForGame();
}

void Container::load() {
    GUI::load();
    bindControls();

    string btnMessage(_game->services().resource().strings().get(kSwitchToResRef) + " " + _game->services().resource().strings().get(kGiveItemResRef));
    _binding.btnGiveItems->setTextMessage(btnMessage);

    string lblMessage(_game->services().resource().strings().get(kInventoryResRef));
    _binding.lblMessage->setTextMessage(lblMessage);

    configureItemsListBox();
}

void Container::bindControls() {
    _binding.lblMessage = getControlPtr<Label>("LBL_MESSAGE");
    _binding.lbItems = getControlPtr<ListBox>("LB_ITEMS");
    _binding.btnOk = getControlPtr<Button>("BTN_OK");
    _binding.btnGiveItems = getControlPtr<Button>("BTN_GIVEITEMS");
    _binding.btnCancel = getControlPtr<Button>("BTN_CANCEL");
}

void Container::configureItemsListBox() {
    ImageButton &protoItem = static_cast<ImageButton &>(_binding.lbItems->protoItem());

    Control::Text text(protoItem.text());
    text.align = Control::TextAlign::LeftTop;

    protoItem.setText(text);
}

void Container::open(shared_ptr<SpatialObject> container) {
    _binding.lbItems->clearItems();

    for (auto &item : container->items()) {
        if (!item->isDropable()) continue;

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

shared_ptr<Texture> Container::getItemFrameTexture(int stackSize) const {
    string resRef;
    if (isTSL(_game->gameId())) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return _game->services().graphics().textures().get(resRef, TextureUsage::GUI);
}

void Container::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_OK") {
        transferItemsToPlayer();
        _game->openInGame();
    } else if (control == "BTN_CANCEL") {
        _game->openInGame();
    }
}

void Container::transferItemsToPlayer() {
    shared_ptr<Creature> player(_game->services().party().player());
    _container->moveDropableItemsTo(*player);

    auto placeable = ObjectConverter::toPlaceable(_container);
    if (placeable) {
        placeable->runOnInvDisturbed(player);
    }
}

} // namespace game

} // namespace reone
