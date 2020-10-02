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

#include "container.h"

#include "../../gui/control/imagebutton.h"
#include "../../gui/control/listbox.h"
#include "../../resource/resources.h"

#include "../object/item.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const int kSwitchToResRef = 47884;
static const int kGiveItemResRef = 47885;
static const int kInventoryResRef = 393;

ContainerGui::ContainerGui(const GraphicsOptions &opts) : GUI(opts) {
}

void ContainerGui::load(GameVersion version) {
    string resRef("container");
    if (version == GameVersion::TheSithLords) {
        _resolutionX = 305;
        _resolutionY = 327;
        resRef += "_p";
    }
    GUI::load(resRef, BackgroundType::None);

    string btnMessage(Resources.getString(kSwitchToResRef).text + " " + Resources.getString(kGiveItemResRef).text);
    getControl("BTN_GIVEITEMS").setTextMessage(btnMessage);

    string lblMessage(Resources.getString(kInventoryResRef).text);
    getControl("LBL_MESSAGE").setTextMessage(lblMessage);

    configureItemsListBox(version);
}

void ContainerGui::configureItemsListBox(GameVersion version) {
    ListBox &listBox = static_cast<ListBox &>(getControl("LB_ITEMS"));
    ImageButton &protoItem = static_cast<ImageButton &>(listBox.protoItem());

    Control::Text text(protoItem.text());
    text.align = Control::TextAlign::LeftCenter;

    protoItem.setText(text);

    string frameTex;
    if (version == GameVersion::TheSithLords) {
        frameTex = "uibit_eqp_itm1";
    } else {
        frameTex = "lbl_hex_3";
    }
    protoItem.setIconFrame(Resources.findTexture(frameTex, TextureType::GUI));
}

void ContainerGui::open(SpatialObject *container) {
    assert(container);
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

SpatialObject &ContainerGui::container() const {
    return *_container;
}

void ContainerGui::onClick(const string &control) {
    if (control == "BTN_OK") {
        if (_onGetItems) {
            _onGetItems();
        }
    } else if (control == "BTN_CANCEL") {
        if (_onClose) {
            _onClose();
        }
    }
}

void ContainerGui::setOnGetItems(const function<void()> &fn) {
    _onGetItems = fn;
}

void ContainerGui::setOnClose(const function<void()> &fn) {
    _onClose = fn;
}

} // namespace game

} // namespace reone
