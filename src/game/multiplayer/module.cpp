#include "module.h"

#include "area.h"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

MultiplayerModule::MultiplayerModule(const std::string &name, GameVersion version, const GraphicsOptions &opts, IMultiplayerCallbacks *callbacks) :
    Module(name, version, opts), _callbacks(callbacks) {
}

const std::shared_ptr<Area> MultiplayerModule::makeArea() const {
    return std::shared_ptr<Area>(new MultiplayerArea(_version, _info.entryArea, _callbacks));
}

} // namespace game

} // namespace reone
