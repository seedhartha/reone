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

#include "../../../src/game/object/creature.h"
#include "../../../src/game/services.h"
#include "../../../src/resource/gff.h"

#include "../../fixtures/functional.h"
#include "../../fixtures/game.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(creature)

BOOST_AUTO_TEST_CASE(should_load_from_git) {
    // given

    setLogLevel(LogLevel::None);

    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto creature = static_pointer_cast<Creature>(game->newCreature());

    auto git = make_shared<Gff>(
        0,
        vector<Gff::Field> {
            Gff::Field::newResRef("TemplateResRef", "some_creature")});

    auto utc = make_shared<Gff>(
        1,
        vector<Gff::Field> {
            Gff::Field::newList(
                "ItemList",
                vector<shared_ptr<Gff>> {
                    make_shared<Gff>(
                        2,
                        vector<Gff::Field> {
                            Gff::Field::newResRef("InventoryRes", "some_item")})})});

    auto appearanceTwoDa = make_shared<TwoDa>(vector<string>(), vector<TwoDa::Row>());
    test.services().resource.twoDas.add("appearance", appearanceTwoDa);

    test.services().resource.gffs.add(ResourceId("some_creature", ResourceType::Utc), utc);

    // when
    creature->loadFromGit(*git);

    // then
    auto &items = creature->items();
    BOOST_CHECK_EQUAL(1ll, items.size());
}

BOOST_AUTO_TEST_SUITE_END()
