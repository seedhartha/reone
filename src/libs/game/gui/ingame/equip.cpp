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

#include "reone/game/gui/ingame/equip.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/ingame.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/item.h"
#include "reone/game/party.h"
#include "reone/graphics/textures.h"
#include "reone/resource/strings.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefNone = 363;

static std::unordered_map<Equipment::Slot, std::string> g_slotNames = {
    {Equipment::Slot::Implant, "IMPLANT"},
    {Equipment::Slot::Head, "HEAD"},
    {Equipment::Slot::Hands, "HANDS"},
    {Equipment::Slot::ArmL, "ARM_L"},
    {Equipment::Slot::Body, "BODY"},
    {Equipment::Slot::ArmR, "ARM_R"},
    {Equipment::Slot::WeapL, "WEAP_L"},
    {Equipment::Slot::Belt, "BELT"},
    {Equipment::Slot::WeapR, "WEAP_R"},
    {Equipment::Slot::WeapL2, "WEAP_L2"},
    {Equipment::Slot::WeapR2, "WEAP_R2"}};

static std::unordered_map<Equipment::Slot, int32_t> g_slotStrRefs = {
    {Equipment::Slot::Implant, 31388},
    {Equipment::Slot::Head, 31375},
    {Equipment::Slot::Hands, 31383},
    {Equipment::Slot::ArmL, 31376},
    {Equipment::Slot::Body, 31380},
    {Equipment::Slot::ArmR, 31377},
    {Equipment::Slot::WeapL, 31378},
    {Equipment::Slot::Belt, 31382},
    {Equipment::Slot::WeapR, 31379},
    {Equipment::Slot::WeapL2, 31378},
    {Equipment::Slot::WeapR2, 31379}};

void Equipment::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    if (_controls.LBL_BAR1)
        _lblBar.push_back(_controls.LBL_BAR1);
    if (_controls.LBL_BAR2)
        _lblBar.push_back(_controls.LBL_BAR2);
    if (_controls.LBL_BAR3)
        _lblBar.push_back(_controls.LBL_BAR3);
    if (_controls.LBL_BAR4)
        _lblBar.push_back(_controls.LBL_BAR4);
    if (_controls.LBL_BAR5)
        _lblBar.push_back(_controls.LBL_BAR5);

    for (auto &slotName : g_slotNames) {
        if ((slotName.first == Slot::WeapL2 || slotName.first == Slot::WeapR2) && !_game.isTSL())
            continue;
        _lblInv[slotName.first] = findControl<Label>("LBL_INV_" + slotName.second);
        _btnInv[slotName.first] = findControl<Button>("BTN_INV_" + slotName.second);
    }

    _controls.BTN_CHANGE1->setFocusable(false);
    _controls.BTN_CHANGE2->setFocusable(false);
    // _controls.btnCharLeft->setVisible(false);
    // _controls.btnCharRight->setVisible(false);
    _controls.LB_DESC->setVisible(false);
    _controls.LBL_CANTEQUIP->setVisible(false);

    configureItemsListBox();

    _controls.BTN_EQUIP->setOnClick([this]() {
        if (_selectedSlot == Slot::None) {
            _game.openInGame();
        } else {
            selectSlot(Slot::None);
        }
    });
    _controls.BTN_BACK->setOnClick([this]() {
        if (_selectedSlot == Slot::None) {
            _game.openInGame();
        } else {
            selectSlot(Slot::None);
        }
    });

    for (auto &slotButton : _btnInv) {
        slotButton.second->setOnClick([&]() {
            selectSlot(slotButton.first);
        });
        slotButton.second->setOnFocusChanged([&](bool focus) {
            if (!focus)
                return;

            std::string slotDesc;

            auto maybeStrRef = g_slotStrRefs.find(slotButton.first);
            if (maybeStrRef != g_slotStrRefs.end()) {
                slotDesc = _services.resource.strings.getText(maybeStrRef->second);
            }

            _controls.LBL_SLOTNAME->setTextMessage(slotDesc);
        });
    }
}

void Equipment::configureItemsListBox() {
    _controls.LB_ITEMS->changeProtoItemType(ControlType::ImageButton);
    _controls.LB_ITEMS->setPadding(5);
    _controls.LB_ITEMS->setOnItemClick([this](const std::string &item) {
        onItemsListBoxItemClick(item);
    });

    auto &protoItem = static_cast<ImageButton &>(_controls.LB_ITEMS->protoItem());
    protoItem.setBorderColor(_baseColor);
    protoItem.setHilightColor(_hilightColor);
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
        throw std::invalid_argument("Equipment: invalid slot: " + std::to_string(static_cast<int>(slot)));
    }
}

void Equipment::onItemsListBoxItemClick(const std::string &item) {
    if (_selectedSlot == Slot::None)
        return;

    std::shared_ptr<Creature> player(_game.party().player());
    std::shared_ptr<Item> itemObj;
    if (item != "[none]") {
        for (auto &playerItem : player->items()) {
            if (playerItem->tag() == item) {
                itemObj = playerItem;
                break;
            }
        }
    }
    int slot = getInventorySlot(_selectedSlot);
    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
    std::shared_ptr<Item> equipped(partyLeader->getEquippedItem(slot));

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
                    std::shared_ptr<Item> clonedItem = _game.newItem();
                    clonedItem->loadFromBlueprint(itemObj->blueprintResRef());
                    partyLeader->equip(slot, clonedItem);
                }
            }
        }
        updateEquipment();
        selectSlot(Slot::None);
    }
}

void Equipment::update() {
    updatePortraits();
    updateEquipment();
    selectSlot(Slot::None);

    auto partyLeader(_game.party().getLeader());

    if (!_game.isTSL()) {
        std::string vitalityString(str(boost::format("%d/\n%d") % partyLeader->currentHitPoints() % partyLeader->hitPoints()));
        _controls.LBL_VITALITY->setTextMessage(vitalityString);
    }
    _controls.LBL_DEF->setTextMessage(std::to_string(partyLeader->getDefense()));
}

void Equipment::updatePortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> partyLeader(party.getLeader());
    std::shared_ptr<Creature> partyMember1(party.getMember(1));
    std::shared_ptr<Creature> partyMember2(party.getMember(2));

    _controls.LBL_PORTRAIT->setBorderFill(partyLeader->portrait());
    _controls.BTN_CHANGE1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _controls.BTN_CHANGE2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void Equipment::selectSlot(Slot slot) {
    bool noneSelected = slot == Slot::None;

    for (auto &lbl : _lblInv) {
        lbl.second->setVisible(noneSelected);
        lbl.second->setVisible(noneSelected);
    }

    _controls.LB_DESC->setVisible(!noneSelected);
    _controls.LBL_SLOTNAME->setVisible(noneSelected);

    if (!_game.isTSL()) {
        _controls.LBL_PORT_BORD->setVisible(noneSelected);
        _controls.LBL_PORTRAIT->setVisible(noneSelected);
        _controls.LBL_TXTBAR->setVisible(noneSelected);
    }
    _selectedSlot = slot;

    updateItems();
}

void Equipment::updateEquipment() {
    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
    auto &equipment = partyLeader->equipment();

    for (auto &lbl : _lblInv) {
        int slot = getInventorySlot(lbl.first);
        std::shared_ptr<Texture> fill;

        auto equipped = equipment.find(slot);
        if (equipped != equipment.end()) {
            fill = equipped->second->icon();
        } else {
            fill = getEmptySlotIcon(lbl.first);
        }

        lbl.second->setBorderFill(fill);
    }

    int min, max;
    partyLeader->getMainHandDamage(min, max);
    _controls.LBL_ATKR->setTextMessage(str(boost::format("%d-%d") % min % max));

    partyLeader->getOffhandDamage(min, max);
    _controls.LBL_ATKL->setTextMessage(str(boost::format("%d-%d") % min % max));

    int attackBonus = partyLeader->getAttackBonus();
    std::string attackBonusString(std::to_string(attackBonus));
    if (attackBonus > 0) {
        attackBonusString.insert(0, "+");
    }
    _controls.LBL_TOHITL->setTextMessage(attackBonusString);
    _controls.LBL_TOHITR->setTextMessage(attackBonusString);
}

std::shared_ptr<Texture> Equipment::getEmptySlotIcon(Slot slot) const {
    static std::unordered_map<Slot, std::shared_ptr<Texture>> icons;

    auto icon = icons.find(slot);
    if (icon != icons.end())
        return icon->second;

    std::string resRef;
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

    std::shared_ptr<Texture> texture(_services.graphics.textures.get(resRef, TextureUsage::GUI));
    auto pair = icons.insert(std::make_pair(slot, texture));

    return pair.first->second;
}

void Equipment::updateItems() {
    _controls.LB_ITEMS->clearItems();

    if (_selectedSlot != Slot::None) {
        ListBox::Item lbItem;
        lbItem.tag = "[none]";
        lbItem.text = _services.resource.strings.getText(kStrRefNone);
        lbItem.iconTexture = _services.graphics.textures.get("inone", TextureUsage::GUI);
        lbItem.iconFrame = getItemFrameTexture(1);

        _controls.LB_ITEMS->addItem(std::move(lbItem));
    }
    std::shared_ptr<Creature> player(_game.party().player());

    for (auto &item : player->items()) {
        if (_selectedSlot == Slot::None) {
            if (!item->isEquippable())
                continue;
        } else {
            int slot = getInventorySlot(_selectedSlot);
            if (!item->isEquippable(slot))
                continue;
        }
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
}

std::shared_ptr<Texture> Equipment::getItemFrameTexture(int stackSize) const {
    std::string resRef;
    if (_game.isTSL()) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return _services.graphics.textures.get(resRef, TextureUsage::GUI);
}

} // namespace game

} // namespace reone
