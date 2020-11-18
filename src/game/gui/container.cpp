/*
 * Copyright (c) 2020 The reone project contributors
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
#include "../script/util.h"

#include "colors.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const int kSwitchToResRef = 47884;
static const int kGiveItemResRef = 47885;
static const int kInventoryResRef = 393;

Container::Container(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("container");

    if (game->version() == GameVersion::TheSithLords) {
        _resolutionX = 305;
        _resolutionY = 327;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = getHilightColor(_version);
    }
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

    string frameTex;
    if (_version == GameVersion::TheSithLords) {
        frameTex = "uibit_eqp_itm1";
    } else {
        frameTex = "lbl_hex_3";
    }
    protoItem.setIconFrame(Textures::instance().get(frameTex, TextureType::GUI));
}

void Container::open(SpatialObject *container) {
    _container = container;

    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clear();

    for (auto &item : container->items()) {
        const ItemBlueprint &blueprint = item->blueprint();

        ListBox::Item lbItem;
        lbItem.tag = blueprint.tag();
        lbItem.icon = blueprint.icon();
        lbItem.text = blueprint.localizedName();

        lbItems.add(move(lbItem));
    }
}

SpatialObject &Container::container() const {
    return *_container;
}

void Container::onClick(const string &control) {
    if (control == "BTN_OK") {
        transferItemsToPlayer();
        _game->openInGame();
    } else if (control == "BTN_CANCEL") {
        _game->openInGame();
    }
}

void Container::transferItemsToPlayer() {
    shared_ptr<Creature> player(_game->party().player());
    _container->moveItemsTo(*player);

    Placeable *placeable = dynamic_cast<Placeable *>(_container);
    if (placeable) {
        string script;
        if (placeable->blueprint().getScript(PlaceableBlueprint::ScriptType::OnInvDisturbed, script)) {
            runScript(script, placeable->id(), player->id(), -1);
        }
    }
}

} // namespace game

} // namespace reone
