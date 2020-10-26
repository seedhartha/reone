/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "portraits.h"

#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

string findPortrait(int appearance) {
    shared_ptr<TwoDaTable> table(Resources::instance().get2DA("portraits"));

    const TwoDaRow *row = table->findRow([&appearance](const TwoDaRow &row) {
        int appearanceNumber = row.getInt("appearancenumber");
        int appearanceS = row.getInt("appearance_s");
        int appearanceL = row.getInt("appearance_l");

        return
            appearanceNumber == appearance ||
            appearanceS == appearance ||
            appearanceL == appearance;
    });

    return row ? row->getString("baseresref") : "";
}

} // namespace game

} // namespace reone
