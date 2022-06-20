/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include <boost/test/unit_test.hpp>

#include "../../src/game/conversation.h"
#include "../../src/resource/talktable.h"

#include "../fixtures/functional.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(conversation)

BOOST_AUTO_TEST_CASE(should_load) {
    // given
    auto talkTable = TalkTable::Builder()
                         .string("Bad StrRef")
                         .string("Entry 1")
                         .string("Entry 2")
                         .string("Reply 1")
                         .string("Reply 2")
                         .string("Reply 3")
                         .build();

    auto dlg = Gff::Builder()
                   .type(0)
                   .field(Gff::Field::newList(
                       "EntryList",
                       vector<shared_ptr<Gff>> {
                           Gff::Builder()
                               .field(Gff::Field::newStrRef("Text", 1))
                               .field(Gff::Field::newList(
                                   "RepliesList",
                                   vector<shared_ptr<Gff>> {
                                       Gff::Builder()
                                           .field(Gff::Field::newInt("Index", 0))
                                           .build(),
                                       Gff::Builder()
                                           .field(Gff::Field::newInt("Index", 1))
                                           .build()}))
                               .build(),
                           Gff::Builder()
                               .field(Gff::Field::newStrRef("Text", 2))
                               .field(Gff::Field::newList(
                                   "RepliesList",
                                   vector<shared_ptr<Gff>> {
                                       Gff::Builder()
                                           .field(Gff::Field::newInt("Index", 2))
                                           .build()}))
                               .build()}))
                   .field(Gff::Field::newList(
                       "ReplyList",
                       vector<shared_ptr<Gff>> {
                           Gff::Builder()
                               .field(Gff::Field::newStrRef("Text", 3))
                               .field(Gff::Field::newList(
                                   "EntriesList",
                                   vector<shared_ptr<Gff>> {
                                       Gff::Builder()
                                           .field(Gff::Field::newInt("Index", 1))
                                           .build()}))
                               .build(),
                           Gff::Builder()
                               .field(Gff::Field::newStrRef("Text", 4))
                               .field(Gff::Field::newList(
                                   "EntriesList",
                                   vector<shared_ptr<Gff>>()))
                               .build(),
                           Gff::Builder()
                               .field(Gff::Field::newStrRef("Text", 5))
                               .field(Gff::Field::newList(
                                   "EntriesList",
                                   vector<shared_ptr<Gff>>()))
                               .build()}))
                   .build();

    auto test = FunctionalTest();
    auto resourceSvc = test.services().resource;
    resourceSvc.strings.setTalkTable(talkTable);
    resourceSvc.gffs.add(ResourceId("some_conversation", ResourceType::Dlg), dlg);

    auto conversation = Conversation(resourceSvc);

    // when
    conversation.load("some_conversation");

    // then
    auto &entries = conversation.entries();
    BOOST_REQUIRE_EQUAL(2ll, entries.size());
    BOOST_CHECK_EQUAL(string("Entry 1"), entries[0].text);
    BOOST_REQUIRE_EQUAL(2ll, entries[0].replies.size());
    BOOST_CHECK_EQUAL(0, entries[0].replies[0].index);
    BOOST_CHECK_EQUAL(1, entries[0].replies[1].index);
    BOOST_CHECK_EQUAL(string("Entry 2"), entries[1].text);
    BOOST_REQUIRE_EQUAL(1ll, entries[1].replies.size());
    BOOST_CHECK_EQUAL(2, entries[1].replies[0].index);
    auto &replies = conversation.replies();
    BOOST_REQUIRE_EQUAL(3ll, replies.size());
    BOOST_CHECK_EQUAL(string("Reply 1"), replies[0].text);
    BOOST_CHECK_EQUAL(1ll, replies[0].entries.size());
    BOOST_CHECK_EQUAL(1, replies[0].entries[0].index);
    BOOST_CHECK_EQUAL(string("Reply 2"), replies[1].text);
    BOOST_REQUIRE_EQUAL(0ll, replies[1].entries.size());
    BOOST_CHECK_EQUAL(string("Reply 3"), replies[2].text);
    BOOST_REQUIRE_EQUAL(0ll, replies[2].entries.size());
}

BOOST_AUTO_TEST_SUITE_END()
