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
#include "../../resources/resources.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

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

    configureListBox(version);
}

void ContainerGui::configureListBox(GameVersion version) {
    ListBox &listBox = static_cast<ListBox &>(getControl("LB_ITEMS"));
    ImageButton &protoItem = static_cast<ImageButton &>(listBox.protoItem());

    Control::Text text(protoItem.text());
    text.align = Control::TextAlign::LeftCenter;

    protoItem.setText(text);

    if (version == GameVersion::KotOR) {
        protoItem.setIconFrame(Resources.findTexture("lbl_hex", TextureType::GUI));
    }
}

void ContainerGui::openContainer(const Placeable &placeable) {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clearItems();

    for (auto &item : placeable.items()) {
        const ItemBlueprint &blueprint = item->getTemplate();
        lbItems.add({ blueprint.resRef(), blueprint.localizedName(), blueprint.icon() });
    }
}

void ContainerGui::onClick(const string &control) {
    if (control == "BTN_CANCEL") {
        if (_onCancel) _onCancel();
    }
}

void ContainerGui::setOnCancel(const function<void()> &fn) {
    _onCancel = fn;
}

} // namespace game

} // namespace reone
