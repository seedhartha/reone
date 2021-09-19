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

#include "randomutil.h"

using namespace std;

namespace reone {

static default_random_engine g_generator(static_cast<uint32_t>(time(nullptr)));

int random(int min, int max) {
    uniform_int_distribution<int> dist(min, max);
    return dist(g_generator);
}

float random(float min, float max) {
    uniform_real_distribution<float> distr(min, max);
    return distr(g_generator);
}

} // namespace reone
