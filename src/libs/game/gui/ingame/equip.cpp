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

#include "reone/graphics/textures.h"
#include "reone/resource/strings.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/ingame.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/factory.h"
#include "reone/game/object/item.h"
#include "reone/game/party.h"

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

    _binding.btnChange1->setFocusable(false);
    _binding.btnChange2->setFocusable(false);
    _binding.btnCharLeft->setVisible(false);
    _binding.btnCharRight->setVisible(false);
    _binding.lbDesc->setVisible(false);
    _binding.lblCantEquip->setVisible(false);

    configureItemsListBox();

    _binding.btnEquip->setOnClick([this]() {
        if (_selectedSlot == Slot::None) {
            _game.openInGame();
        } else {
            selectSlot(Slot::None);
        }
    });
    _binding.btnBack->setOnClick([this]() {
        if (_selectedSlot == Slot::None) {
            _game.openInGame();
        } else {
            selectSlot(Slot::None);
        }
    });

    for (auto &slotButton : _binding.btnInv) {
        slotButton.second->setOnClick([&]() {
            selectSlot(slotButton.first);
        });
        slotButton.second->setOnFocusChanged([&](bool focus) {
            if (!focus)
                return;

            std::string slotDesc;

            auto maybeStrRef = g_slotStrRefs.find(slotButton.first);
            if (maybeStrRef != g_slotStrRefs.end()) {
                slotDesc = _services.resource.strings.get(maybeStrRef->second);
            }

            _binding.lblSlotName->setTextMessage(slotDesc);
        });
    }
}

void Equipment::bindControls() {
    _binding.lblCantEquip = findControl<Label>("LBL_CANTEQUIP");
    if (!_game.isTSL()) {
        _binding.lblAttackInfo = findControl<Label>("LBL_ATTACK_INFO");
        _binding.lblPortBord = findControl<Label>("LBL_PORT_BORD");
        _binding.lblPortrait = findControl<Label>("LBL_PORTRAIT");
        _binding.lblDefInfo = findControl<Label>("LBL_DEF_INFO");
        _binding.lblTxtBar = findControl<Label>("LBL_TXTBAR");
        _binding.lblVitality = findControl<Label>("LBL_VITALITY");
        _binding.lblSelectTitle = findControl<Label>("LBL_SELECTTITLE");
        _binding.btnChange1 = findControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = findControl<Button>("BTN_CHANGE2");
        _binding.btnCharLeft = findControl<Button>("BTN_CHARLEFT");
        _binding.btnCharRight = findControl<Button>("BTN_CHARRIGHT");
    } else {
        _binding.lblBack1 = findControl<Label>("LBL_BACK1");
        _binding.lblDefBack = findControl<Label>("LBL_DEF_BACK");
        for (int i = 0; i < kNumControlsBar; ++i) {
            _binding.lblBar[i] = findControl<Label>("LBL_BAR" + std::to_string(i + 1));
        }
        _binding.lblAttackMod = findControl<Label>("LBL_ATTACKMOD");
        _binding.lblDamText = findControl<Label>("LBL_DAMTEXT");
        _binding.btnSwapWeapons = findControl<Button>("BTN_SWAPWEAPONS");
        _binding.btnPrevNpc = findControl<Button>("BTN_PREVNPC");
        _binding.btnNextNpc = findControl<Button>("BTN_NEXTNPC");
        _binding.lblDefText = findControl<Label>("LBL_DEF_TEXT");
        _binding.btnChange1 = _inGameMenu.getBtnChange2();
        _binding.btnChange2 = _inGameMenu.getBtnChange3();
        _binding.btnCharLeft = findControl<Button>("BTN_PREVNPC");
        _binding.btnCharRight = findControl<Button>("BTN_NEXTNPC");
    }
    _binding.lblToHitR = findControl<Label>("LBL_TOHITR");
    _binding.lbItems = findControl<ListBox>("LB_ITEMS");
    for (auto &slotName : g_slotNames) {
        if ((slotName.first == Slot::WeapL2 || slotName.first == Slot::WeapR2) && !_game.isTSL())
            continue;
        _binding.lblInv[slotName.first] = findControl<Label>("LBL_INV_" + slotName.second);
        _binding.btnInv[slotName.first] = findControl<Button>("BTN_INV_" + slotName.second);
    }
    _binding.lblAtkL = findControl<Label>("LBL_ATKL");
    _binding.lblAtkR = findControl<Label>("LBL_ATKR");
    _binding.lblDef = findControl<Label>("LBL_DEF");
    _binding.lblTitle = findControl<Label>("LBL_TITLE");
    _binding.lblDamage = findControl<Label>("LBL_DAMAGE");
    _binding.lblToHitL = findControl<Label>("LBL_TOHITL");
    _binding.lblToHit = findControl<Label>("LBL_TOHIT");
    _binding.lblSlotName = findControl<Label>("LBL_SLOTNAME");
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnEquip = findControl<Button>("BTN_EQUIP");
    _binding.lbDesc = findControl<ListBox>("LB_DESC");
}

void Equipment::configureItemsListBox() {
    _binding.lbItems->changeProtoItemType(ControlType::ImageButton);
    _binding.lbItems->setPadding(5);
    _binding.lbItems->setOnItemClick([this](const std::string &item) {
        onItemsListBoxItemClick(item);
    });

    auto &protoItem = static_cast<ImageButton &>(_binding.lbItems->protoItem());
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
                    std::shared_ptr<Item> clonedItem = _game.objectFactory().newItem();
                    _game.addObject(clonedItem);
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
        _binding.lblVitality->setTextMessage(vitalityString);
    }
    _binding.lblDef->setTextMessage(std::to_string(partyLeader->getDefense()));
}

void Equipment::updatePortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> partyLeader(party.getLeader());
    std::shared_ptr<Creature> partyMember1(party.getMember(1));
    std::shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.lblPortrait->setBorderFill(partyLeader->portrait());
    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void Equipment::selectSlot(Slot slot) {
    bool noneSelected = slot == Slot::None;

    for (auto &lbl : _binding.lblInv) {
        lbl.second->setVisible(noneSelected);
        lbl.second->setVisible(noneSelected);
    }

    _binding.lbDesc->setVisible(!noneSelected);
    _binding.lblSlotName->setVisible(noneSelected);

    if (!_game.isTSL()) {
        _binding.lblPortBord->setVisible(noneSelected);
        _binding.lblPortrait->setVisible(noneSelected);
        _binding.lblTxtBar->setVisible(noneSelected);
    }
    _selectedSlot = slot;

    updateItems();
}

void Equipment::updateEquipment() {
    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
    auto &equipment = partyLeader->equipment();

    for (auto &lbl : _binding.lblInv) {
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
    _binding.lblAtkR->setTextMessage(str(boost::format("%d-%d") % min % max));

    partyLeader->getOffhandDamage(min, max);
    _binding.lblAtkL->setTextMessage(str(boost::format("%d-%d") % min % max));

    int attackBonus = partyLeader->getAttackBonus();
    std::string attackBonusString(std::to_string(attackBonus));
    if (attackBonus > 0) {
        attackBonusString.insert(0, "+");
    }
    _binding.lblToHitL->setTextMessage(attackBonusString);
    _binding.lblToHitR->setTextMessage(attackBonusString);
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
    _binding.lbItems->clearItems();

    if (_selectedSlot != Slot::None) {
        ListBox::Item lbItem;
        lbItem.tag = "[none]";
        lbItem.text = _services.resource.strings.get(kStrRefNone);
        lbItem.iconTexture = _services.graphics.textures.get("inone", TextureUsage::GUI);
        lbItem.iconFrame = getItemFrameTexture(1);

        _binding.lbItems->addItem(std::move(lbItem));
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
        _binding.lbItems->addItem(std::move(lbItem));
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
