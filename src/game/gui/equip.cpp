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

#include "equip.h"

#include "../../gui/control/imagebutton.h"
#include "../../gui/control/listbox.h"
#include "../../resources/resources.h"

#include "../object/item.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

EquipmentGui::EquipmentGui(const GraphicsOptions &opts) : GUI(opts) {
}

void EquipmentGui::load(GameVersion version) {
    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
    GUI::load(getResRef(version), BackgroundType::Menu);

    hideControl("LB_DESC");
    hideControl("LBL_CANTEQUIP");

    configureItemsListBox(version);
}

string EquipmentGui::getResRef(GameVersion version) const {
    string resRef("equip");
    if (version == GameVersion::TheSithLords) {
        resRef += "_p";
    }

    return resRef;
};

void EquipmentGui::configureItemsListBox(GameVersion version) {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.setPadding(5);

    ImageButton &protoItem = static_cast<ImageButton &>(lbItems.protoItem());

    Control::Border border(protoItem.border());
    border.color = getBaseColor(version);
    protoItem.setBorder(border);

    Control::Border hilight(protoItem.hilight());
    hilight.color = getHilightColor(version);
    protoItem.setHilight(hilight);

    if (version == GameVersion::KotOR) {
        protoItem.setIconFrame(Resources.findTexture("lbl_hex_3", TextureType::GUI));
    }
}

void EquipmentGui::open(SpatialObject *owner) {
    assert(owner);
    _owner = owner;

    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clear();

    for (auto &item : owner->items()) {
        const ItemBlueprint &blueprint = item->blueprint();

        ListBox::Item lbItem;
        lbItem.tag = blueprint.resRef();
        lbItem.text = blueprint.localizedName();
        lbItem.icon = blueprint.icon();

        lbItems.add(move(lbItem));
    }
}

void EquipmentGui::setOnClose(const function<void()> &fn) {
    _onClose = fn;
}

void EquipmentGui::configureControl(Control &control) {
    if (control.tag() == "LB_ITEMS") {
        static_cast<ListBox &>(control).setProtoItemType(ControlType::ImageButton);
    }
}

void EquipmentGui::onClick(const string &control) {
    if (control == "BTN_BACK") {
        if (_onClose) {
            _onClose();
        }
    }
}

} // namespace game

} // namespace reone
