#pragma once

#include "../module.h"

namespace reone {

namespace game {

class IMultiplayerCallbacks;

class MultiplayerModule : public Module {
public:
    MultiplayerModule(const std::string &name, resources::GameVersion version, const render::GraphicsOptions &opts, IMultiplayerCallbacks *callbacks);

private:
    IMultiplayerCallbacks *_callbacks { nullptr };

    const std::shared_ptr<Area> makeArea() const override;
};

} // namespace game

} // namespace reone
