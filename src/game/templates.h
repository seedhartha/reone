#pragma once

#include <memory>
#include <string>

#include "item.h"

namespace reone {

namespace game {

class TemplateManager {
public:
    static TemplateManager &instance();

    std::shared_ptr<Item> findItem(const std::string &resRef);

private:
    TemplateManager() = default;

    TemplateManager(const TemplateManager &) = delete;
    TemplateManager &operator=(const TemplateManager &) = delete;
};

} // namespace game

} // namespace reone
