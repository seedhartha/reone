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

#define BOOST_TEST_MODULE gffwriter

#include <sstream>

#include <boost/filesystem/fstream.hpp>
#include <boost/test/included/unit_test.hpp>

#include "../src/resource/format/gfffile.h"
#include "../src/resource/format/gffwriter.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_CASE(test_save_utw) {
    vector<shared_ptr<GffStruct::Field>> fields {
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Appearance"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoString, "LinkedTo"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "TemplateResRef"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoString, "Tag"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoLocString, "LocalizedName"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoLocString, "Description"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "HasMapNote"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoLocString, "MapNote"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "MapNoteEnabled"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "PaletteID"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoString, "Comment")
    };

    fields[2]->strValue = "testwp";
    fields[3]->strValue = "testwp";

    auto gff = make_shared<GffStruct>();
    for (auto &field : fields) {
        gff->add(field);
    }

    auto out = make_shared<ostringstream>();
    GffWriter writer(ResourceType::Utw, gff);
    writer.save(out);

    string result(out->str());
    BOOST_TEST((result.substr(0, 8) == "UTW V3.2"));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[8]) == 0x38));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[12]) == 1));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[16]) == 0x44));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[20]) == 11));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[24]) == 0xc8));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[28]) == 11));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[32]) == 0x178));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[36]) == 61));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[40]) == 0x1b5));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[44]) == 44));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[48]) == 0x1e1));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[52]) == 0));
}

BOOST_AUTO_TEST_CASE(test_save_utp) {
    vector<shared_ptr<GffStruct::Field>> item1Fields {
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "InventoryRes"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Word, "Repos_PosX"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Word, "Repos_Posy")
    };
    item1Fields[0]->strValue = "g_i_credits002";
    auto item1 = make_shared<GffStruct>();
    for (auto &field : item1Fields) {
        item1->add(field);
    }

    vector<shared_ptr<GffStruct::Field>> item2Fields {
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "InventoryRes"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Word, "Repos_PosX"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Word, "Repos_Posy")
    };
    item2Fields[0]->strValue = "g_i_medeqpmnt01";
    auto item2 = make_shared<GffStruct>();
    for (auto &field : item2Fields) {
        item2->add(field);
    }

    vector<shared_ptr<GffStruct::Field>> rootFields {
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoString, "Tag"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoLocString, "LocName"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoLocString, "Description"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "TemplateResRef"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "AutoRemoveKey"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "CloseLockDC"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "Conversation"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Interruptable"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Dword, "Faction"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Plot"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Min1HP"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "KeyRequired"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Lockable"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Locked"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "OpenLockDC"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Word, "PortraitId"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "TrapDetectable"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "TrapDetectDC"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "TrapDisarmable"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "DisarmDC"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "TrapFlag"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "TrapOneShot"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "TrapType"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoString, "KeyName"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "AnimationState"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Dword, "Appearance"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Short, "HP"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Short, "CurrentHP"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Hardness"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Fort"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Ref"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Will"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnClosed"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnDamaged"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnDeath"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnDisarm"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnHeartbeat"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnLock"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnMeleeAttacked"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnOpen"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnSpellCastAt"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnTrapTriggered"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnUnlock"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnUserDefined"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "HasInventory"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "PartyInteract"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "BodyBag"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Static"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Type"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "Useable"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnEndDialogue"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnInvDisturbed"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::ResRef, "OnUsed"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::List, "ItemList"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "PaletteID"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::CExoString, "Comment")
    };
    rootFields[0]->strValue = "MetalBox";
    rootFields[3]->strValue = "metalbox001";
    rootFields[53]->children.push_back(move(item1));
    rootFields[53]->children.push_back(move(item2));
    rootFields[55]->strValue = "Contains:\r\n10 cr\r\nMedpack";
    auto root = make_shared<GffStruct>();
    for (auto &field : rootFields) {
        root->add(field);
    }

    auto out = make_shared<ostringstream>();
    GffWriter writer(ResourceType::Utp, root);
    writer.save(out);

    string result(out->str());
    BOOST_TEST((result.substr(0, 8) == "UTP V3.2"));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[8]) == 0x38));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[12]) == 3));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[16]) == 0x5c));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[20]) == 62));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[24]) == 0x344));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[28]) == 59));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[32]) == 0x6f4));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[36]) == 128));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[40]) == 0x774));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[44]) == 248));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[48]) == 0x86c));
    BOOST_TEST((*reinterpret_cast<uint32_t *>(&result[52]) == 12));
}

BOOST_AUTO_TEST_CASE(test_save_load) {
    auto structChild = make_shared<GffStruct>();
    auto structChildField = make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "MyByte");
    structChildField->uintValue = 1;
    structChild->add(move(structChildField));

    auto listChild1 = make_shared<GffStruct>();
    auto listChild1Field = make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "MyByte");
    listChild1Field->uintValue = 2;
    listChild1->add(move(listChild1Field));

    auto listChild2 = make_shared<GffStruct>();
    auto listChild2Field = make_shared<GffStruct::Field>(GffStruct::FieldType::Byte, "MyByte");
    listChild2Field->uintValue = 3;
    listChild2->add(move(listChild2Field));

    vector<shared_ptr<GffStruct::Field>> rootFields {
        make_shared<GffStruct::Field>(GffStruct::FieldType::Struct, "MyStruct"),
        make_shared<GffStruct::Field>(GffStruct::FieldType::List, "MyList"),
    };
    rootFields[0]->children.push_back(move(structChild));
    rootFields[1]->children.push_back(move(listChild1));
    rootFields[1]->children.push_back(move(listChild2));
    auto root = make_shared<GffStruct>();
    for (auto &field : rootFields) {
        root->add(field);
    }

    auto out = make_shared<ostringstream>();
    GffWriter writer(ResourceType::Utp, root);
    writer.save(out);

    auto in = make_shared<istringstream>(out->str());
    GffFile gffFile;
    gffFile.load(in);
    auto readRoot = gffFile.root();

    BOOST_TEST((readRoot->fields().size() == 2ll));
    BOOST_TEST((readRoot->fields()[0]->children.size() == 1ll));
    BOOST_TEST((readRoot->fields()[1]->children.size() == 2ll));
    BOOST_TEST((readRoot->fields()[0]->children[0]->fields()[0]->uintValue == 1));
    BOOST_TEST((readRoot->fields()[1]->children[0]->fields()[0]->uintValue == 2));
    BOOST_TEST((readRoot->fields()[1]->children[1]->fields()[0]->uintValue == 3));
}
