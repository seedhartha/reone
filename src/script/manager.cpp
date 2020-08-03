#include "manager.h"

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
    auto pair = g_programs.insert(std::make_pair(name, program));

    return pair.first->second;
}

} // namespace script

} // namespace reone
