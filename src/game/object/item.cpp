/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "item.h"

#include "../../common/exception/validation.h"
#include "../../graphics/models.h"
#include "../../graphics/services.h"
#include "../../graphics/textures.h"
#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"
#include "../../resource/strings.h"
#include "../../scene/graph.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Item::loadFromUti(const string &templateResRef) {
    // From UTI
    auto uti = _resourceSvc.gffs.get(templateResRef, ResourceType::Uti);
    if (!uti) {
        throw ValidationException("UTC not found: " + templateResRef);
    }
    auto tag = uti->getString("Tag");
    auto name = _resourceSvc.strings.get(uti->getInt("LocalizedName"));
    auto baseItem = uti->getInt("BaseItem");
    auto modelVariation = uti->getInt("ModelVariation", 1);
    auto bodyVariation = uti->getInt("BodyVariation", 1);
    auto textureVar = uti->getInt("TextureVar", 1);

    // From baseitems 2DA
    auto baseItems = _resourceSvc.twoDas.get("baseitems");
    if (!baseItems) {
        throw ValidationException("baseitems 2DA not found");
    }
    auto equipableSlots = baseItems->getUint(baseItem, "equipableslots");
    auto modelType = baseItems->getInt(baseItem, "modeltype");
    auto itemClass = boost::to_lower_copy(baseItems->getString(baseItem, "itemclass"));
    auto defaultModel = boost::to_lower_copy(baseItems->getString(baseItem, "defaultmodel"));
    auto weaponWield = baseItems->getInt(baseItem, "weaponwield");
    auto weaponType = baseItems->getInt(baseItem, "weapontype");

    if (modelType == 0 && defaultModel != "i_null") {
        auto modelResRef = str(boost::format("%s_%03d") % itemClass % modelVariation);
        auto model = _graphicsSvc.models.get(modelResRef);
        if (!model) {
            model = _graphicsSvc.models.get(defaultModel);
        }
        if (model) {
            auto modelSceneNode = _sceneGraph->newModel(*model, ModelUsage::Equipment);
            modelSceneNode->init();
            _sceneNode = modelSceneNode.get();
        }
    }

    _tag = move(tag);
    _name = move(name);
    _equipableSlots = equipableSlots;
    _bodyVariation = bodyVariation;
    _textureVar = textureVar;
}

} // namespace game

} // namespace reone
