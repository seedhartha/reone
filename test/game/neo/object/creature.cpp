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

#include <gtest/gtest.h>

#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/item.h"
#include "reone/resource/2da.h"
#include "reone/resource/template/generated/utc.h"

using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource;
using namespace reone::resource::generated;

TEST(creature, should_load_utc) {
    // given
    Creature creature {0, ""};
    UTC utc;
    utc.Appearance_Type = 0;
    utc.BodyVariation = 1;
    utc.TextureVar = 1;
    TwoDA appearance2da {
        {"race", "racetex"},
        {TwoDA::newRow({"n_mandalorian", "n_mandalorian01"})}};
    TwoDA head2da {
        {"head"},
        {}};

    // when
    creature.load(utc, appearance2da, heads2da);

    // then
    EXPECT_TRUE(creature.is(ObjectState::Loaded));
    EXPECT_EQ(creature.appearance().model, "n_selkath");
    EXPECT_EQ(creature.appearance().texture, "n_selkath");
}

TEST(creature, should_load_utc_with_body_and_head_appearance) {
    // given
    Creature creature {0, ""};
    UTC utc;
    utc.Appearance_Type = 0;
    utc.BodyVariation = 1;
    utc.TextureVar = 1;
    TwoDA appearance2da {
        {"modela", "texa"},
        {TwoDA::newRow({"pmbal", "pmbal"})}};
    TwoDA head2da {
        {"head"},
        {TwoDA::newRow("pmhc01")}};

    // when
    creature.load(utc, appearance2da, heads2da);

    // then
    EXPECT_EQ(creature.appearance().model, "pmbal");
    EXPECT_EQ(creature.appearance().texture, "pmbal01");
}

TEST(creature, should_equip_item) {
    // given
    Creature creature {0, ""};
    Item item {1, ""};

    // when
    creature.equip(item, InventorySlots::rightWeapon);

    // then
    auto &equipment = creature.equipment();
    EXPECT_EQ(equipment.count(InventorySlots::rightWeapon), 1);
    EXPECT_EQ(equipment.at(InventorySlots::rightWeapon), item);
}

TEST(creature, should_unequip_previously_equipped_item_on_equip) {
    // given
    Creature creature {0, ""};
    Item item {1, ""};
    Item item2 {2, ""};
    creature.equip(item, InventorySlots::rightWeapon);

    // when
    creature.equip(item2, InventorySlots::rightWeapon);

    // then
    auto &equipment = creature.equipment();
    EXPECT_EQ(equipment.count(InventorySlots::rightWeapon), 1);
    EXPECT_EQ(equipment.at(InventorySlots::rightWeapon), item2);
    auto &items = creature.items();
    EXPECT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().get(), item);
}

TEST(creature, should_unequip_item) {
    // given
    Creature creature {0, ""};
    Item item {1, ""};
    creature.equip(item, InventorySlots::rightWeapon);

    // when
    creature.unequip(item);

    // then
    auto &equipment = creature.equipment();
    EXPECT_EQ(equipment.count(InventorySlots::rightWeapon), 0);
    auto &items = creature.items();
    EXPECT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().get(), item);
}

TEST(creature, should_give_item) {
    // given
    Creature creature {0, ""};
    Item item {1, ""};

    // when
    creature.give(item);

    // then
    auto &items = creature.items();
    EXPECT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().get(), item);
}
