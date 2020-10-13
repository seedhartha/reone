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

#include <boost/algorithm/string.hpp>

#include "../../gui/control/imagebutton.h"
#include "../../gui/control/listbox.h"
#include "../../resource/resources.h"

#include "../object/creature.h"
#include "../object/item.h"

#include "colors.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const int kStrRefNone = 363;

static unordered_map<EquipmentGui::Slot, string> g_slotNames = {
    { EquipmentGui::Slot::Implant, "IMPLANT"},
    { EquipmentGui::Slot::Head, "HEAD"},
    { EquipmentGui::Slot::Hands, "HANDS"},
    { EquipmentGui::Slot::ArmL, "ARM_L"},
    { EquipmentGui::Slot::Body, "BODY"},
    { EquipmentGui::Slot::ArmR, "ARM_R"},
    { EquipmentGui::Slot::WeapL, "WEAP_L"},
    { EquipmentGui::Slot::Belt, "BELT"},
    { EquipmentGui::Slot::WeapR, "WEAP_R"},
    { EquipmentGui::Slot::WeapL2, "WEAP_L2"},
    { EquipmentGui::Slot::WeapR2, "WEAP_R2"}
};

EquipmentGui::EquipmentGui(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("equip");
    _backgroundType = BackgroundType::Menu;

    if (_version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void EquipmentGui::load() {
    GUI::load();

    hideControl("LB_DESC");
    hideControl("LBL_CANTEQUIP");

    configureItemsListBox();
}

void EquipmentGui::configureItemsListBox() {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.setPadding(5);
    lbItems.setOnItemClicked(bind(&EquipmentGui::onItemClicked, this, _1, _2));

    ImageButton &protoItem = static_cast<ImageButton &>(lbItems.protoItem());
    protoItem.setBorderColor(getBaseColor(_version));
    protoItem.setHilightColor(getHilightColor(_version));

    string frameTex;
    if (_version == GameVersion::TheSithLords) {
        frameTex = "uibit_eqp_itm1";
    } else {
        frameTex = "lbl_hex_3";
    }
    protoItem.setIconFrame(Resources.findTexture(frameTex, TextureType::GUI));
}

void EquipmentGui::onItemClicked(const string &control, const string &item) {
    if (control != "LB_ITEMS" || _selectedSlot == Slot::None) return;

    shared_ptr<Item> itemObj;

    if (item != "[none]") {
        for (auto &ownerItem : _owner->items()) {
            if (ownerItem->tag() == item) {
                itemObj = ownerItem;
                break;
            }
        }
    }

    Creature &owner = static_cast<Creature &>(*_owner);
    InventorySlot slot = getInventorySlot(_selectedSlot);
    shared_ptr<Item> equipped = owner.getEquippedItem(slot);

    if (equipped != itemObj) {
        if (equipped) {
            owner.unequip(equipped);
        }
        if (itemObj) {
            owner.equip(slot, itemObj);
        }
        updateEquipment();
        selectSlot(Slot::None);
    }
}

void EquipmentGui::open(SpatialObject *owner) {
    _owner = owner;

    updateEquipment();
    selectSlot(Slot::None);
}

void EquipmentGui::setOnClose(const function<void()> &fn) {
    _onClose = fn;
}

void EquipmentGui::preloadControl(Control &control) {
    if (control.tag() == "LB_ITEMS") {
        static_cast<ListBox &>(control).setProtoItemType(ControlType::ImageButton);
    }
}

void EquipmentGui::onClick(const string &control) {
    if (control == "BTN_EQUIP" || control == "BTN_BACK") {
        if (_selectedSlot == Slot::None) {
            if (_onClose) {
                _onClose();
            }
        } else {
            selectSlot(Slot::None);
        }
    } else if (boost::starts_with(control, "BTN_INV_")) {
        for (auto &name : g_slotNames) {
            string slotName(control.substr(8));
            if (name.second == slotName) {
                selectSlot(name.first);
                break;
            }
        }
    }
}

void EquipmentGui::selectSlot(Slot slot) {
    bool noneSelected = slot == Slot::None;

    for (auto &name : g_slotNames) {
        configureControl("LBL_INV_" + name.second, [&noneSelected](Control &control) { control.setVisible(noneSelected); });
        configureControl("BTN_INV_" + name.second, [&noneSelected](Control &control) { control.setVisible(noneSelected); });
    }

    getControl("LB_DESC").setVisible(!noneSelected);
    getControl("LBL_SLOTNAME").setVisible(noneSelected);

    if (_version == GameVersion::KotOR) {
        getControl("LBL_PORT_BORD").setVisible(noneSelected);
        getControl("LBL_PORTRAIT").setVisible(noneSelected);
        getControl("LBL_TXTBAR").setVisible(noneSelected);
    }
    _selectedSlot = slot;

    updateItems();
}

void EquipmentGui::updateEquipment() {
    Creature &owner = static_cast<Creature &>(*_owner);
    const map<InventorySlot, shared_ptr<Item>> &equipment = owner.equipment();

    for (auto &name : g_slotNames) {
        string tag("LBL_INV_" + name.second);
        configureControl(tag, [&name, &equipment](Control &control) {
            InventorySlot slot = getInventorySlot(name.first);
            Control::Border border(control.border());

            auto equipped = equipment.find(slot);
            if (equipped != equipment.end()) {
                border.fill = equipped->second->blueprint().icon();
            } else {
                border.fill = getEmptySlotIcon(name.first);
            }
            control.setBorder(border);
        });
    }
}

InventorySlot EquipmentGui::getInventorySlot(Slot slot) {
    switch (slot) {
        case Slot::Implant:
            return InventorySlot::kInventorySlotImplant;
        case Slot::Head:
            return InventorySlot::kInventorySlotHead;
        case Slot::Hands:
            return InventorySlot::kInventorySlotHands;
        case Slot::ArmL:
            return InventorySlot::kInventorySlotLeftArm;
        case Slot::Body:
            return InventorySlot::kInventorySlotBody;
        case Slot::ArmR:
            return InventorySlot::kInventorySlotRightArm;
        case Slot::WeapL:
            return InventorySlot::kInventorySlotLeftWeapon;
        case Slot::Belt:
            return InventorySlot::kInventorySlotBelt;
        case Slot::WeapR:
            return InventorySlot::kInventorySlotRightWeapon;
        case Slot::WeapL2:
            return InventorySlot::kInventorySlotLeftWeapon2;
        case Slot::WeapR2:
            return InventorySlot::kInventorySlotRightWeapon2;
        default:
            throw invalid_argument("Equipment: invalid slot: " + to_string(static_cast<int>(slot)));
    }
}

shared_ptr<Texture> EquipmentGui::getEmptySlotIcon(Slot slot) {
    static unordered_map<Slot, shared_ptr<Texture>> icons;

    auto icon = icons.find(slot);
    if (icon != icons.end()) return icon->second;

    string resRef;
    switch (slot) {
        case Slot::Implant:
            resRef = "iimplant";
            break;
        case Slot::Head:
            resRef = "ihead";
            break;
        case Slot::Hands:
            resRef = "ihands";
            break;
        case Slot::ArmL:
            resRef = "iforearm_l";
            break;
        case Slot::Body:
            resRef = "iarmor";
            break;
        case Slot::ArmR:
            resRef = "iforearm_r";
            break;
        case Slot::WeapL:
        case Slot::WeapL2:
            resRef = "iweap_l";
            break;
        case Slot::Belt:
            resRef = "ibelt";
            break;
        case Slot::WeapR:
        case Slot::WeapR2:
            resRef = "iweap_r";
            break;
        default:
            return nullptr;
    }

    shared_ptr<Texture> texture(Resources.findTexture(resRef, TextureType::GUI));
    auto pair = icons.insert(make_pair(slot, texture));

    return pair.first->second;
}

void EquipmentGui::updateItems() {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clear();

    if (_selectedSlot != Slot::None) {
        ListBox::Item lbItem;
        lbItem.tag = "[none]";
        lbItem.text = Resources.getString(kStrRefNone).text;
        lbItem.icon = Resources.findTexture("inone", TextureType::GUI);

        lbItems.add(move(lbItem));
    }
    for (auto &item : _owner->items()) {
        const ItemBlueprint &blueprint = item->blueprint();

        if (_selectedSlot == Slot::None) {
            if (!blueprint.isEquippable()) continue;
        } else {
            InventorySlot slot = getInventorySlot(_selectedSlot);
            if (!blueprint.isEquippable(slot)) continue;
        }
        ListBox::Item lbItem;
        lbItem.tag = blueprint.tag();
        lbItem.text = blueprint.localizedName();
        lbItem.icon = blueprint.icon();

        lbItems.add(move(lbItem));
    }
}

} // namespace game

} // namespace reone
