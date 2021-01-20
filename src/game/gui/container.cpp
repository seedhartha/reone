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

#include "../../gui/control/imagebutton.h"
#include "../../gui/control/listbox.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"

#include "../game.h"
#include "../object/item.h"

#include "colorutil.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kSwitchToResRef = 47884;
static constexpr int kGiveItemResRef = 47885;
static constexpr int kInventoryResRef = 393;

Container::Container(Game *game) :
    GameGUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("container");

    initForGame();
}

void Container::load() {
    GUI::load();

    string btnMessage(Resources::instance().getString(kSwitchToResRef) + " " + Resources::instance().getString(kGiveItemResRef));

    Control &btnGiveItems = getControl("BTN_GIVEITEMS");
    btnGiveItems.setTextMessage(btnMessage);

    string lblMessage(Resources::instance().getString(kInventoryResRef));
    getControl("LBL_MESSAGE").setTextMessage(lblMessage);

    configureItemsListBox();
}

void Container::configureItemsListBox() {
    ListBox &listBox = static_cast<ListBox &>(getControl("LB_ITEMS"));
    ImageButton &protoItem = static_cast<ImageButton &>(listBox.protoItem());

    Control::Text text(protoItem.text());
    text.align = Control::TextAlign::LeftCenter;

    protoItem.setText(text);
}

void Container::open(const shared_ptr<SpatialObject> &container) {
    _container = container;

    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clearItems();

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
        lbItems.addItem(move(lbItem));
    }
}

shared_ptr<Texture> Container::getItemFrameTexture(int stackSize) const {
    string resRef;
    if (_version == GameVersion::TheSithLords) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return Textures::instance().get(resRef, TextureType::GUI);
}

SpatialObject &Container::container() const {
    return *_container;
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
    shared_ptr<Creature> player(_game->party().player());
    _container->moveDropableItemsTo(*player);

    auto placeable = dynamic_pointer_cast<Placeable>(_container);
    if (placeable) {
        string script(placeable->onInvDisturbed());
        if (!script.empty()) {
            _game->scriptRunner().run(script, placeable->id(), player->id());
        }
    }
}

} // namespace game

} // namespace reone
