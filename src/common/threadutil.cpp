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

#ifdef R_ENABLE_MULTITHREADING
static mutex g_namesMutex;
#endif

static map<thread::id, std::string> g_names;

string getThreadName() {
    thread::id id(this_thread::get_id());

#ifdef R_ENABLE_MULTITHREADING
    lock_guard<mutex> lock(g_namesMutex);
#endif

    return getFromLookupOrElse(g_names, id, [&id]() {
        stringstream ss;
        ss << id;
        return ss.str();
    });
}

void setThreadName(string name) {
#ifdef R_ENABLE_MULTITHREADING
    lock_guard<mutex> lock(g_namesMutex);
#endif

    g_names[this_thread::get_id()] = move(name);
}

} // namespace reone
