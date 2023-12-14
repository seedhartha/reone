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

#include "reone/gui/control/imagebutton.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/strings.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/item.h"
#include "reone/game/object/placeable.h"
#include "reone/game/party.h"

using namespace reone::audio;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kSwitchToResRef = 47884;
static constexpr int kGiveItemResRef = 47885;
static constexpr int kInventoryResRef = 393;

void ContainerGUI::onGUILoaded() {
    bindControls();

    std::string btnMessage(_services.resource.strings.getText(kSwitchToResRef) + " " + _services.resource.strings.getText(kGiveItemResRef));
    _controls.BTN_GIVEITEMS->setTextMessage(btnMessage);

    std::string LBL_MESSAGE(_services.resource.strings.getText(kInventoryResRef));
    _controls.LBL_MESSAGE->setTextMessage(LBL_MESSAGE);

    _controls.BTN_OK->setOnClick([this]() {
        transferItemsToPlayer();
        _game.openInGame();
    });
    _controls.BTN_CANCEL->setOnClick([this]() {
        _game.openInGame();
    });

    configureItemsListBox();
}

void ContainerGUI::configureItemsListBox() {
    ImageButton &protoItem = static_cast<ImageButton &>(_controls.LB_ITEMS->protoItem());

    Control::Text text(protoItem.text());
    text.align = Control::TextAlign::LeftTop;

    protoItem.setText(text);
}

void ContainerGUI::open(std::shared_ptr<Object> container) {
    _controls.LB_ITEMS->clearItems();

    for (auto &item : container->items()) {
        if (!item->isDropable())
            continue;

        ListBox::Item lbItem;
        lbItem.tag = item->tag();
        lbItem.text = item->localizedName();
        lbItem.iconTexture = item->icon();
        lbItem.iconFrame = getItemFrameTexture(item->stackSize());

        if (item->stackSize() > 1) {
            lbItem.iconText = std::to_string(item->stackSize());
        }
        _controls.LB_ITEMS->addItem(std::move(lbItem));
    }

    _container = std::move(container);
}

std::shared_ptr<Texture> ContainerGUI::getItemFrameTexture(int stackSize) const {
    std::string resRef;
    if (_game.isTSL()) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return _services.resource.textures.get(resRef, TextureUsage::GUI);
}

void ContainerGUI::transferItemsToPlayer() {
    std::shared_ptr<Creature> player(_game.party().player());
    _container->moveDropableItemsTo(*player);

    auto placeable = std::dynamic_pointer_cast<Placeable>(_container);
    if (placeable) {
        placeable->runOnInvDisturbed(player);
    }
}

} // namespace game

} // namespace reone
