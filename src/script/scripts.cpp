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

#include "scripts.h"

#include <map>

#include "../core/log.h"
#include "../core/streamutil.h"
#include "../resources/manager.h"
#include "../resources/ncsfile.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace script {

static map<string, shared_ptr<ScriptProgram>> g_programs;

ScriptManager &ScriptManager::instance() {
    static ScriptManager instance;
    return instance;
}

shared_ptr<ScriptProgram> ScriptManager::find(const string &name) {
    auto it = g_programs.find(name);
    if (it != g_programs.end()) return it->second;

    debug("Loading script program " + name);
    shared_ptr<ScriptProgram> program;
    shared_ptr<ByteArray> ncsData(ResMan.find(name, ResourceType::CompiledScript));
    if (ncsData) {
        NcsFile ncs(name);
        ncs.load(wrap(ncsData));
        program = ncs.program();
    }
    auto pair = g_programs.insert(make_pair(name, program));

    return pair.first->second;
}

} // namespace script

} // namespace reone
