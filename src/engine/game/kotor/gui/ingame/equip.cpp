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

#include "../../../../graphics/texture/textures.h"
#include "../../../../resource/strings.h"

#include "../../../core/object/creature.h"
#include "../../../core/object/factory.h"
#include "../../../core/object/item.h"
#include "../../../core/party.h"
#include "../../../core/services.h"

#include "../../kotor.h"

#include "ingame.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kStrRefNone = 363;

static unordered_map<Equipment::Slot, string> g_slotNames = {
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

static unordered_map<Equipment::Slot, int32_t> g_slotStrRefs = {
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

Equipment::Equipment(
    KotOR &game,
    InGameMenu &inGameMenu,
    Services &services) :
    GameGUI(game, services),
    _inGameMenu(inGameMenu) {
    _resRef = getResRef("equip");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void Equipment::load() {
    GUI::load();
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

            string slotDesc;

            auto maybeStrRef = g_slotStrRefs.find(slotButton.first);
            if (maybeStrRef != g_slotStrRefs.end()) {
                slotDesc = _strings.get(maybeStrRef->second);
            }

            _binding.lblSlotName->setTextMessage(slotDesc);
        });
    }
}

void Equipment::bindControls() {
    _binding.lblCantEquip = getControl<Label>("LBL_CANTEQUIP");
    if (!_game.isTSL()) {
        _binding.lblAttackInfo = getControl<Label>("LBL_ATTACK_INFO");
        _binding.lblPortBord = getControl<Label>("LBL_PORT_BORD");
        _binding.lblPortrait = getControl<Label>("LBL_PORTRAIT");
        _binding.lblDefInfo = getControl<Label>("LBL_DEF_INFO");
        _binding.lblTxtBar = getControl<Label>("LBL_TXTBAR");
        _binding.lblVitality = getControl<Label>("LBL_VITALITY");
        _binding.lblSelectTitle = getControl<Label>("LBL_SELECTTITLE");
        _binding.btnChange1 = getControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControl<Button>("BTN_CHANGE2");
        _binding.btnCharLeft = getControl<Button>("BTN_CHARLEFT");
        _binding.btnCharRight = getControl<Button>("BTN_CHARRIGHT");
    } else {
        _binding.lblBack1 = getControl<Label>("LBL_BACK1");
        _binding.lblDefBack = getControl<Label>("LBL_DEF_BACK");
        for (int i = 0; i < kNumControlsBar; ++i) {
            _binding.lblBar[i] = getControl<Label>("LBL_BAR" + to_string(i + 1));
        }
        _binding.lblAttackMod = getControl<Label>("LBL_ATTACKMOD");
        _binding.lblDamText = getControl<Label>("LBL_DAMTEXT");
        _binding.btnSwapWeapons = getControl<Button>("BTN_SWAPWEAPONS");
        _binding.btnPrevNpc = getControl<Button>("BTN_PREVNPC");
        _binding.btnNextNpc = getControl<Button>("BTN_NEXTNPC");
        _binding.lblDefText = getControl<Label>("LBL_DEF_TEXT");
        _binding.btnChange1 = _inGameMenu.getBtnChange2();
        _binding.btnChange2 = _inGameMenu.getBtnChange3();
        _binding.btnCharLeft = getControl<Button>("BTN_PREVNPC");
        _binding.btnCharRight = getControl<Button>("BTN_NEXTNPC");
    }
    _binding.lblToHitR = getControl<Label>("LBL_TOHITR");
    _binding.lbItems = getControl<ListBox>("LB_ITEMS");
    for (auto &slotName : g_slotNames) {
        if ((slotName.first == Slot::WeapL2 || slotName.first == Slot::WeapR2) && !_game.isTSL())
            continue;
        _binding.lblInv[slotName.first] = getControl<Label>("LBL_INV_" + slotName.second);
        _binding.btnInv[slotName.first] = getControl<Button>("BTN_INV_" + slotName.second);
    }
    _binding.lblAtkL = getControl<Label>("LBL_ATKL");
    _binding.lblAtkR = getControl<Label>("LBL_ATKR");
    _binding.lblDef = getControl<Label>("LBL_DEF");
    _binding.lblTitle = getControl<Label>("LBL_TITLE");
    _binding.lblDamage = getControl<Label>("LBL_DAMAGE");
    _binding.lblToHitL = getControl<Label>("LBL_TOHITL");
    _binding.lblToHit = getControl<Label>("LBL_TOHIT");
    _binding.lblSlotName = getControl<Label>("LBL_SLOTNAME");
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnEquip = getControl<Button>("BTN_EQUIP");
    _binding.lbDesc = getControl<ListBox>("LB_DESC");
}

void Equipment::configureItemsListBox() {
    _binding.lbItems->setPadding(5);
    _binding.lbItems->setOnItemClick([this](const string &item) {
        onItemsListBoxItemClick(item);
    });

    ImageButton &protoItem = static_cast<ImageButton &>(_binding.lbItems->protoItem());
    protoItem.setBorderColor(_game.getGUIColorBase());
    protoItem.setHilightColor(_game.getGUIColorHilight());
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

void Equipment::onItemsListBoxItemClick(const string &item) {
    if (_selectedSlot == Slot::None)
        return;

    shared_ptr<Creature> player(_game.party().player());
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
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
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
                    shared_ptr<Item> clonedItem(_game.objectFactory().newItem());
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
        string vitalityString(str(boost::format("%d/\n%d") % partyLeader->currentHitPoints() % partyLeader->hitPoints()));
        _binding.lblVitality->setTextMessage(vitalityString);
    }
    _binding.lblDef->setTextMessage(to_string(partyLeader->getDefense()));
}

void Equipment::updatePortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    shared_ptr<Creature> partyLeader(party.getLeader());
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.lblPortrait->setBorderFill(partyLeader->portrait());
    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void Equipment::preloadControl(Control &control) {
    if (control.tag() == "LB_ITEMS") {
        static_cast<ListBox &>(control).setProtoItemType(ControlType::ImageButton);
    }
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
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
    auto &equipment = partyLeader->equipment();

    for (auto &lbl : _binding.lblInv) {
        int slot = getInventorySlot(lbl.first);
        shared_ptr<Texture> fill;

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
    string attackBonusString(to_string(attackBonus));
    if (attackBonus > 0) {
        attackBonusString.insert(0, "+");
    }
    _binding.lblToHitL->setTextMessage(attackBonusString);
    _binding.lblToHitR->setTextMessage(attackBonusString);
}

shared_ptr<Texture> Equipment::getEmptySlotIcon(Slot slot) const {
    static unordered_map<Slot, shared_ptr<Texture>> icons;

    auto icon = icons.find(slot);
    if (icon != icons.end())
        return icon->second;

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

    shared_ptr<Texture> texture(_textures.get(resRef, TextureUsage::GUI));
    auto pair = icons.insert(make_pair(slot, texture));

    return pair.first->second;
}

void Equipment::updateItems() {
    _binding.lbItems->clearItems();

    if (_selectedSlot != Slot::None) {
        ListBox::Item lbItem;
        lbItem.tag = "[none]";
        lbItem.text = _strings.get(kStrRefNone);
        lbItem.iconTexture = _textures.get("inone", TextureUsage::GUI);
        lbItem.iconFrame = getItemFrameTexture(1);

        _binding.lbItems->addItem(move(lbItem));
    }
    shared_ptr<Creature> player(_game.party().player());

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
            lbItem.iconText = to_string(item->stackSize());
        }
        _binding.lbItems->addItem(move(lbItem));
    }
}

shared_ptr<Texture> Equipment::getItemFrameTexture(int stackSize) const {
    string resRef;
    if (_game.isTSL()) {
        resRef = stackSize > 1 ? "uibit_eqp_itm3" : "uibit_eqp_itm1";
    } else {
        resRef = stackSize > 1 ? "lbl_hex_7" : "lbl_hex_3";
    }
    return _textures.get(resRef, TextureUsage::GUI);
}

} // namespace game

} // namespace reone
