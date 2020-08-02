#pragma once

#include <boost/filesystem.hpp>

#include "glm/glm.hpp"

namespace reone {

namespace resources {

class LytFile {
public:
    struct Room {
        std::string name;
        glm::vec3 position { 0.0f };
    };

    struct DoorHook {
        std::string room;
        std::string name;
        glm::vec3 position { 0.0f };
    };

    LytFile() = default;

    void load(const std::shared_ptr<std::istream> &in);
    void load(const boost::filesystem::path &path);

    const std::vector<Room> &rooms() const;
    const std::vector<DoorHook> &doorHooks() const;

private:
    enum class State {
        None,
        Layout,
        Rooms,
        DoorHooks
    };

    std::shared_ptr<std::istream> _in;
    boost::filesystem::path _path;
    State _state { State::None };
    int _roomCount { 0 };
    int _doorHookCount { 0 };
    std::vector<Room> _rooms;
    std::vector<DoorHook> _doorHooks;

    LytFile(const LytFile &) = delete;
    LytFile &operator=(const LytFile &) = delete;

    void load();
    void processLine(const std::string &line);
    Room getRoom(const std::vector<std::string> &tokens) const;
    DoorHook getDoorHook(const std::vector<std::string> &tokens) const;
};

} // namespace resources

} // namespace reone
