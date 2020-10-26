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
#include "../../render/textures.h"
#include "../../resource/resources.h"

#include "../game.h"
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

static unordered_map<Equipment::Slot, string> g_slotNames = {
    { Equipment::Slot::Implant, "IMPLANT"},
    { Equipment::Slot::Head, "HEAD"},
    { Equipment::Slot::Hands, "HANDS"},
    { Equipment::Slot::ArmL, "ARM_L"},
    { Equipment::Slot::Body, "BODY"},
    { Equipment::Slot::ArmR, "ARM_R"},
    { Equipment::Slot::WeapL, "WEAP_L"},
    { Equipment::Slot::Belt, "BELT"},
    { Equipment::Slot::WeapR, "WEAP_R"},
    { Equipment::Slot::WeapL2, "WEAP_L2"},
    { Equipment::Slot::WeapR2, "WEAP_R2"}
};

Equipment::Equipment(Game *game, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _game(game) {

    _resRef = getResRef("equip");
    _backgroundType = BackgroundType::Menu;

    if (_version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
}

void Equipment::load() {
    GUI::load();

    hideControl("LB_DESC");
    hideControl("LBL_ATKL");
    hideControl("LBL_ATKR");
    hideControl("LBL_CANTEQUIP");
    hideControl("LBL_DEF");
    hideControl("LBL_TOHITL");
    hideControl("LBL_TOHITR");
    hideControl("LBL_VITALITY");

    configureItemsListBox();
}

void Equipment::configureItemsListBox() {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.setPadding(5);

    ImageButton &protoItem = static_cast<ImageButton &>(lbItems.protoItem());
    protoItem.setBorderColor(getBaseColor(_version));
    protoItem.setHilightColor(getHilightColor(_version));

    string frameTex;
    if (_version == GameVersion::TheSithLords) {
        frameTex = "uibit_eqp_itm1";
    } else {
        frameTex = "lbl_hex_3";
    }
    protoItem.setIconFrame(Textures::instance().get(frameTex, TextureType::GUI));
}

static InventorySlot getInventorySlot(Equipment::Slot slot) {
    switch (slot) {
        case Equipment::Slot::Implant:
            return InventorySlot::kInventorySlotImplant;
        case Equipment::Slot::Head:
            return InventorySlot::kInventorySlotHead;
        case Equipment::Slot::Hands:
            return InventorySlot::kInventorySlotHands;
        case Equipment::Slot::ArmL:
            return InventorySlot::kInventorySlotLeftArm;
        case Equipment::Slot::Body:
            return InventorySlot::kInventorySlotBody;
        case Equipment::Slot::ArmR:
            return InventorySlot::kInventorySlotRightArm;
        case Equipment::Slot::WeapL:
            return InventorySlot::kInventorySlotLeftWeapon;
        case Equipment::Slot::Belt:
            return InventorySlot::kInventorySlotBelt;
        case Equipment::Slot::WeapR:
            return InventorySlot::kInventorySlotRightWeapon;
        case Equipment::Slot::WeapL2:
            return InventorySlot::kInventorySlotLeftWeapon2;
        case Equipment::Slot::WeapR2:
            return InventorySlot::kInventorySlotRightWeapon2;
        default:
            throw invalid_argument("Equipment: invalid slot: " + to_string(static_cast<int>(slot)));
    }
}

void Equipment::onListBoxItemClick(const string &control, const string &item) {
    if (control != "LB_ITEMS" || _selectedSlot == Slot::None) return;

    Creature *partyLeader = _game->party().leader();
    shared_ptr<Item> itemObj;
    if (item != "[none]") {
        for (auto &ownerItem : partyLeader->items()) {
            if (ownerItem->tag() == item) {
                itemObj = ownerItem;
                break;
            }
        }
    }
    InventorySlot slot = getInventorySlot(_selectedSlot);
    shared_ptr<Item> equipped = partyLeader->getEquippedItem(slot);

    if (equipped != itemObj) {
        if (equipped) {
            partyLeader->unequip(equipped);
        }
        if (itemObj) {
            partyLeader->equip(slot, itemObj);
        }
        updateEquipment();
        selectSlot(Slot::None);
    }
}

void Equipment::update() {
    updatePortraits();
    updateEquipment();
    selectSlot(Slot::None);
}

void Equipment::updatePortraits() {
    Party &party = _game->party();
    Creature *partyLeader = party.leader();
    Creature *partyMember1 = party.getMember(1);
    Creature *partyMember2 = party.getMember(2);

    Control &lblPortrait = getControl("LBL_PORTRAIT");
    lblPortrait.setBorderFill(partyLeader->portrait());

    Control &btnChange1 = getControl("BTN_CHANGE1");
    btnChange1.setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);

    Control &btnChange2 = getControl("BTN_CHANGE2");
    btnChange2.setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void Equipment::preloadControl(Control &control) {
    if (control.tag() == "LB_ITEMS") {
        static_cast<ListBox &>(control).setProtoItemType(ControlType::ImageButton);
    }
}

void Equipment::onClick(const string &control) {
    if (control == "BTN_EQUIP" || control == "BTN_BACK") {
        if (_selectedSlot == Slot::None) {
            _game->openInGame();
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

void Equipment::selectSlot(Slot slot) {
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

static shared_ptr<Texture> getEmptySlotIcon(Equipment::Slot slot) {
    static unordered_map<Equipment::Slot, shared_ptr<Texture>> icons;

    auto icon = icons.find(slot);
    if (icon != icons.end()) return icon->second;

    string resRef;
    switch (slot) {
        case Equipment::Slot::Implant:
            resRef = "iimplant";
            break;
        case Equipment::Slot::Head:
            resRef = "ihead";
            break;
        case Equipment::Slot::Hands:
            resRef = "ihands";
            break;
        case Equipment::Slot::ArmL:
            resRef = "iforearm_l";
            break;
        case Equipment::Slot::Body:
            resRef = "iarmor";
            break;
        case Equipment::Slot::ArmR:
            resRef = "iforearm_r";
            break;
        case Equipment::Slot::WeapL:
        case Equipment::Slot::WeapL2:
            resRef = "iweap_l";
            break;
        case Equipment::Slot::Belt:
            resRef = "ibelt";
            break;
        case Equipment::Slot::WeapR:
        case Equipment::Slot::WeapR2:
            resRef = "iweap_r";
            break;
        default:
            return nullptr;
    }

    shared_ptr<Texture> texture(Textures::instance().get(resRef, TextureType::GUI));
    auto pair = icons.insert(make_pair(slot, texture));

    return pair.first->second;
}

void Equipment::updateEquipment() {
    Creature *partyLeader = _game->party().leader();
    const map<InventorySlot, shared_ptr<Item>> &equipment = partyLeader->equipment();

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

void Equipment::updateItems() {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clear();

    if (_selectedSlot != Slot::None) {
        ListBox::Item lbItem;
        lbItem.tag = "[none]";
        lbItem.text = Resources::instance().getString(kStrRefNone).text;
        lbItem.icon = Textures::instance().get("inone", TextureType::GUI);

        lbItems.add(move(lbItem));
    }
    Creature *partyLeader = _game->party().leader();

    for (auto &item : partyLeader->items()) {
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
