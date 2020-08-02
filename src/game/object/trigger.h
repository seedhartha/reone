#pragma once

#include "object.h"

namespace reone {

namespace game {

class Trigger : public Object {
public:
    Trigger(uint32_t id);

    void load(const resources::GffStruct &gffs);

    const std::string &linkedToModule() const;
    const std::string &linkedTo() const;
    const std::vector<glm::vec3> &geometry() const;

private:
    std::string _transitionDestin;
    std::string _linkedToModule;
    std::string _linkedTo;
    std::vector<glm::vec3> _geometry;
};

} // namespace game

} // namespace reone
