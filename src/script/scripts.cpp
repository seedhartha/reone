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

#include "../resource/resources.h"
#include "../system/streamutil.h"

#include "ncsfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace script {

Scripts &Scripts::instance() {
    static Scripts instance;
    return instance;
}

void Scripts::invalidateCache() {
    _cache.clear();
}

shared_ptr<ScriptProgram> Scripts::get(const string &resRef) {
    auto maybeModel = _cache.find(resRef);
    if (maybeModel != _cache.end()) {
        return maybeModel->second;
    }
    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef)));

    return inserted.first->second;
}

shared_ptr<ScriptProgram> Scripts::doGet(const string &resRef) {
    shared_ptr<ByteArray> data(Resources::instance().findRaw(resRef, ResourceType::CompiledScript));
    shared_ptr<ScriptProgram> program;

    if (data) {
        NcsFile ncs(resRef);
        ncs.load(wrap(data));
        program = ncs.program();
    }

    return move(program);
}

} // namespace script

} // namespace reone
