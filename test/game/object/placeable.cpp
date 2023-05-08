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

#include "reone/game/object/placeable.h"
#include "reone/resource/gff.h"

#include "../../fixtures/functional.h"
#include "../../fixtures/game.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::resource;

BOOST_AUTO_TEST_SUITE(placeable)

BOOST_AUTO_TEST_CASE(should_load_from_git) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto placeable = static_pointer_cast<Placeable>(game->newPlaceable());

    auto git = make_shared<Gff>(
        0,
        vector<Gff::Field> {
            Gff::Field::newResRef("TemplateResRef", "some_placeable")});

    auto utp = make_shared<Gff>(
        1,
        vector<Gff::Field> {
            Gff::Field::newInt("HasInventory", 1),
            Gff::Field::newList(
                "ItemList",
                vector<shared_ptr<Gff>> {
                    make_shared<Gff>(
                        2,
                        vector<Gff::Field> {
                            Gff::Field::newResRef("InventoryRes", "some_item")})})});

    auto uti = make_shared<Gff>(
        2,
        vector<Gff::Field> {});

    auto placeablesTwoDa = make_shared<TwoDa>(vector<string>(), vector<TwoDa::Row>());
    auto baseItemsTwoDa = make_shared<TwoDa>(vector<string>(), vector<TwoDa::Row>());
    auto &twoDas = test.services().resource.twoDas;
    twoDas.add("placeables", placeablesTwoDa);
    twoDas.add("baseitems", baseItemsTwoDa);

    auto &gffs = test.services().resource.gffs;
    gffs.add(ResourceId("some_placeable", ResourceType::Utp), utp);
    gffs.add(ResourceId("some_item", ResourceType::Uti), uti);

    // when
    placeable->loadFromGit(*git);

    // then
    auto &items = placeable->items();
    BOOST_TEST(1ll == items.size());
}

BOOST_AUTO_TEST_SUITE_END()
