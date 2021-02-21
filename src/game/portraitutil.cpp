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

#include "portraitutil.h"

#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

string getPortrait(int id) {
    shared_ptr<TwoDA> portraits(Resources::instance().get2DA("portraits"));
    return portraits->getString(id, "baseresref");
}

string getPortraitByAppearance(int appearance) {
    vector<pair<string, string>> columnValues {
        { "appearancenumber", to_string(appearance) },
        { "appearance_s", to_string(appearance) },
        { "appearance_l", to_string(appearance) }
    };
    shared_ptr<TwoDA> portraits(Resources::instance().get2DA("portraits"));

    int row = portraits->indexByCellValuesAny(columnValues);
    if (row == -1) return "";

    return portraits->getString(row, "baseresref");
}

} // namespace game

} // namespace reone
