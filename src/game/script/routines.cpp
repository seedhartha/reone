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

#include "routines.h"

using namespace reone::resources;

namespace reone {

namespace game {

extern void initKotorRoutines();
extern void initTslRoutines();

void initScriptRoutines(GameVersion version) {
    switch (version) {
        case GameVersion::TheSithLords:
            initTslRoutines();
            break;
        default:
            initKotorRoutines();
            break;
    }
}

} // namespace game

} // namespace reone
