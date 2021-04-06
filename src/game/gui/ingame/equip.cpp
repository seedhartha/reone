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

#include "equip.h"

#include <boost/algorithm/string.hpp>

#include "../../../gui/control/imagebutton.h"
#include "../../../gui/control/listbox.h"
#include "../../../render/textures.h"
#include "../../../resource/strings.h"

#include "../../game.h"
#include "../../gameidutil.h"
#include "../../object/creature.h"
#include "../../object/item.h"

#include "../colorutil.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefNone = 363;

static unordered_map<Equipment::Slot, string> g_slotNames = {
    { Equipment::Slot::Implant, "IMPLANT" },
    { Equipment::Slot::Head, "HEAD" },
    { Equipment::Slot::Hands, "HANDS" },
    { Equipment::Slot::ArmL, "ARM_L" },
    { Equipment::Slot::Body, "BODY" },
    { Equipment::Slot::ArmR, "ARM_R" },
    { Equipment::Slot::WeapL, "WEAP_L" },
    { Equipment::Slot::Belt, "BELT" },
    { Equipment::Slot::WeapR, "WEAP_R" },
    { Equipment::Slot::WeapL2, "WEAP_L2" },
    { Equipment::Slot::WeapR2, "WEAP_R2" }
};

static unordered_map<Equipment::Slot, int32_t> g_slotStrRefs = {
    { Equipment::Slot::Implant, 31388 },
    { Equipment::Slot::Head, 31375 },
    { Equipment::Slot::Hands, 31383 },
    { Equipment::Slot::ArmL, 31376 },
    { Equipment::Slot::Body, 31380 },
    { Equipment::Slot::ArmR, 31377 },
    { Equipment::Slot::WeapL, 31378 },
    { Equipment::Slot::Belt, 31382 },
    { Equipment::Slot::WeapR, 31379 },
    { Equipment::Slot::WeapL2, 31378 },
    { Equipment::Slot::WeapR2, 31379 }
};

Equipment::Equipment(Game *game) :
    GameGUI(game->gameId(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("equip");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void Equipment::load() {
    GUI::load();

    hideControl("BTN_CHARLEFT");
    hideControl("BTN_CHARRIGHT");
    hideControl("LB_DESC");
    hideControl("LBL_ATKL");
    hideControl("LBL_ATKR");
    hideControl("LBL_CANTEQUIP");
    hideControl("LBL_DEF");
    hideControl("LBL_TOHITL");
    hideControl("LBL_TOHITR");
    hideControl("LBL_VITALITY");

    setControlFocusable("BTN_CHANGE1", false);
    setControlFocusable("BTN_CHANGE2", false);

    configureItemsListBox();
}

void Equipment::configureItemsListBox() {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.setPadding(5);

    ImageButton &protoItem = static_cast<ImageButton &>(lbItems.protoItem());
    protoItem.setBorderColor(getBaseColor(_gameId));
    protoItem.setHilightColor(getHilightColor(_gameId));
}

static int getInventorySlot(Equipment::Slot slot) {
    switch (slot) {
        case Equipment::Slot::Implant:
            return InventorySlot::implant;
        case Equipment::Slot::Head:
            return InventorySlot::head;
        case Equipment::Slot::Hands:
            return InventorySlot::hands;
        case Equipment::Slot::ArmL:
            return InventorySlot::leftArm;
        case Equipment::Slot::Body:
            return InventorySlot::body;
        case Equipment::Slot::ArmR:
            return InventorySlot::rightArm;
        case Equipment::Slot::WeapL:
            return InventorySlot::leftWeapon;
        case Equipment::Slot::Belt:
            return InventorySlot::belt;
        case Equipment::Slot::WeapR:
            return InventorySlot::rightWeapon;
        case Equipment::Slot::WeapL2:
            return InventorySlot::leftWeapon2;
        case Equipment::Slot::WeapR2:
            return InventorySlot::rightWeapon2;
        default:
            throw invalid_argument("Equipment: invalid slot: " + to_string(static_cast<int>(slot)));
    }
}

void Equipment::onListBoxItemClick(const string &control, const string &item) {
    if (control != "LB_ITEMS" || _selectedSlot == Slot::None) return;

    shared_ptr<Creature> player(_game->party().player());
    shared_ptr<Item> itemObj;
    if (item != "[none]") {
        for (auto &playerItem : player->items()) {
            if (playerItem->tag() == item) {
                itemObj = playerItem;
                break;
            }
        }
    }
    int slot = getInventorySlot(_selectedSlot);
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    shared_ptr<Item> equipped(partyLeader->getEquippedItem(slot));

    if (equipped != itemObj) {
        if (equipped) {
            partyLeader->unequip(equipped);
            player->addItem(equipped);
        }
        if (itemObj) {
            bool last;
            if (player->removeItem(itemObj, last)) {
                if (last) {
                    partyLeader->equip(slot, itemObj);
                } else {
                    shared_ptr<Item> clonedItem(_game->objectFactory().newItem());
                    clonedItem->loadFromBlueprint(itemObj->blueprintResRef());
                    partyLeader->equip(slot, clonedItem);
                }
            }
        }
        updateEquipment();
        selectSlot(Slot::None);
    }
}

void Equipment::onFocusChanged(const string &control, bool focus) {
    GUI::onFocusChanged(control, focus);

    if (!focus || !boost::starts_with(control, "BTN_INV_")) return;

    string slotDesc;

    string slotName(control.substr(8));
    for (auto &name : g_slotNames) {
        if (name.second != slotName)  continue;

        auto maybeStrRef = g_slotStrRefs.find(name.first);
        if (maybeStrRef != g_slotStrRefs.end()) {
            slotDesc = Strings::instance().get(maybeStrRef->second);
        }
        break;
    }

    getControl("LBL_SLOTNAME").setTextMessage(slotDesc);
}

void Equipment::update() {
    updatePortraits();
    updateEquipment();
    selectSlot(Slot::None);
}

void Equipment::updatePortraits() {
    if (_gameId != GameID::KotOR) return;

    Party &party = _game->party();
    shared_ptr<Creature> partyLeader(party.getLeader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

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
    GameGUI::onClick(control);

    if (control == "BTN_EQUIP" || control == "BTN_BACK") {
        if (_selectedSlot == Slot::None) {
            _game->openInGame();
        } else {
            selectSlot(Slot::None);
        }
    } else if (boost::starts_with(control, "BTN_INV_")) {
        string slotName(control.substr(8));
        for (auto &name : g_slotNames) {
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
        setControlVisible("LBL_INV_" + name.second, noneSelected);
        setControlVisible("BTN_INV_" + name.second, noneSelected);
    }

    setControlVisible("LB_DESC", !noneSelected);
    setControlVisible("LBL_SLOTNAME", noneSelected);

    if (_gameId == GameID::KotOR) {
        setControlVisible("LBL_PORT_BORD", noneSelected);
        setControlVisible("LBL_PORTRAIT", noneSelected);
        setControlVisible("LBL_TXTBAR", noneSelected);
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

    shared_ptr<Texture> texture(Textures::instance().get(resRef, TextureUsage::GUI));
    auto pair = icons.insert(make_pair(slot, texture));

    return pair.first->second;
}

void Equipment::updateEquipment() {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    auto &equipment = partyLeader->equipment();

    for (auto &name : g_slotNames) {
        string tag("LBL_INV_" + name.second);
        configureControl(tag, [&name, &equipment](Control &control) {
            int slot = getInventorySlot(name.first);
            shared_ptr<Texture> fill;

            auto equipped = equipment.find(slot);
            if (equipped != equipment.end()) {
                fill = equipped->second->icon();
            } else {
                fill = getEmptySlotIcon(name.first);
            }

            control.setBorderFill(fill);
        });
    }
}

void Equipment::updateItems() {
    ListBox &lbItems = static_cast<ListBox &>(getControl("LB_ITEMS"));
    lbItems.clearItems();

    if (_selectedSlot != Slot::None) {
        ListBox::Item lbItem;
        lbItem.tag = "[none]";
        lbItem.text = Strings::instance().get(kStrRefNone);
        lbItem.iconTexture = Textures::instance().get("inone", TextureUsage::GUI);
        lbItem.iconFrame = getItemFrameTexture(1);

        lbItems.addItem(move(lbItem));
    }
    shared_ptr<Creature> player(_game->party().player());

    for (auto &item : player->items()) {
        if (_selectedSlot == Slot::None) {
            if (!item->isEquippable()) continue;
        } else {
            int slot = getInventorySlot(_selectedSlot);
            if (!item->isEquippable(slot)) continue;
        }
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

shared_ptr<Texture> Equipment::getItemFrameTexture(int stackSize) const {
    string resRef;
    if (isTSL(_gameId)) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return Textures::instance().get(resRef, TextureUsage::GUI);
}

} // namespace game

} // namespace reone
