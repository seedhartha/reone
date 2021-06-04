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

/** @file
 *  Combat functions related to damage calculation.
 */

#include "combat.h"

#include "../../common/random.h"

using namespace std;

namespace reone {

namespace game {

vector<shared_ptr<DamageEffect>> Combat::getDamageEffects(shared_ptr<Creature> damager, bool offHand, float multiplier) const {
    shared_ptr<Item> weapon(damager->getEquippedItem(offHand ? InventorySlot::leftWeapon : InventorySlot::rightWeapon));
    int amount = 0;
    auto type = DamageType::Bludgeoning;

    if (weapon) {
        for (int i = 0; i < weapon->numDice(); ++i) {
            amount += random(1, weapon->dieToRoll());
        }
        type = static_cast<DamageType>(weapon->damageFlags());
    }
    amount = glm::max(1, amount);
    auto effect = make_shared<DamageEffect>(multiplier * amount, type, move(damager));

    return vector<shared_ptr<DamageEffect>> { effect };
}

} // namespace game

} // namespace reone
