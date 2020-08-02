#pragma once

#include "object.h"

namespace reone {

namespace game {

class Door : public Object {
public:
    Door(uint32_t id);

    void load(const resources::GffStruct &gffs);
    virtual void open(const std::shared_ptr<Object> &trigerrer);
    void saveTo(AreaState &state) const override;
    void loadState(const AreaState &state) override;

    bool isOpen() const;
    bool isStatic() const;
    const std::string &linkedToModule() const;
    const std::string &linkedTo() const;
    const std::string &transitionDestin() const;

private:
    bool _open { false };
    bool _static { false };
    std::string _linkedToModule;
    std::string _linkedTo;
    std::string _transitionDestin;

    void loadBlueprint(const resources::GffStruct &gffs);
};

} // namespace game

} // namespace reone
