#include "templates.h"

#include <map>

#include "../core/log.h"
#include "../resources/manager.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace game {

static map<string, shared_ptr<Item>> g_itemCache = map<string, shared_ptr<Item>>();

TemplateManager &TemplateManager::instance() {
    static TemplateManager instance;
    return instance;
}

shared_ptr<Item> TemplateManager::findItem(const string &resRef) {
    auto it = g_itemCache.find(resRef);
    if (it != g_itemCache.end()) {
        return it->second;
    }
    debug("Loading item blueprint " + resRef);

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<GffStruct> uti(resources.findGFF(resRef, ResourceType::ItemBlueprint));
    shared_ptr<Item> item;

    if (uti) {
        item.reset(new Item());
        item->load(resRef, *uti);
    } else {
        warn("Item blueprint not found: " + resRef);
    }

    auto pair = g_itemCache.insert(std::make_pair(resRef, item));

    return pair.first->second;
}

} // namespace game

} // namespace reone
