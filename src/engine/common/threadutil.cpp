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

#include "threadutil.h"

#include "collectionutil.h"

using namespace std;

namespace reone {

static map<thread::id, std::string> g_threadNames;

string getThreadName() {
    thread::id id(this_thread::get_id());
    return getFromLookupOrElse(g_threadNames, id, [&id]() {
        stringstream ss;
        ss << id;
        return ss.str();
    });
}

void setThreadName(string name) {
    g_threadNames[this_thread::get_id()] = move(name);
}

} // namespace reone
