/*
 * Copyright (c) 2020 The reone project contributors
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

#include "damageresolver.h"

#include "../../common/random.h"

#include "../enginetype/effect.h"
#include "../object/creature.h"
#include "../types.h"

using namespace std;

namespace reone {

namespace game {

vector<shared_ptr<DamageEffect>> DamageResolver::getDamageEffects(const shared_ptr<Creature> &damager) {
    shared_ptr<Item> item(damager->getEquippedItem(InventorySlot::kInventorySlotRightWeapon));
    int amount = 0;
    DamageType type = DamageType::Bludgeoning;

    if (item) {
        for (int i = 0; i < item->numDice(); ++i) {
            amount += random(1, item->dieToRoll());
        }
        type = static_cast<DamageType>(item->damageFlags());
    }
    amount = glm::max(1, amount);
    shared_ptr<DamageEffect> effect(make_shared<DamageEffect>(amount, type, damager));

    return vector<shared_ptr<DamageEffect>> { effect };
}

} // namespace game

} // namespace reone
