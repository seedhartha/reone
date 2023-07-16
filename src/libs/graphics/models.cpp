/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/models.h"

#include "reone/graphics/format/mdlmdxreader.h"
#include "reone/graphics/model.h"
#include "reone/graphics/textures.h"
#include "reone/resource/exception/format.h"
#include "reone/resource/resources.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

Models::Models(Textures &textures, Resources &resources) :
    _textures(textures), _resources(resources) {
}

void Models::clear() {
    _cache.clear();
}

std::shared_ptr<Model> Models::get(const std::string &resRef) {
    if (resRef.empty()) {
        return nullptr;
    }

    auto lcResRef = boost::to_lower_copy(resRef);
    auto maybeModel = _cache.find(lcResRef);
    if (maybeModel != _cache.end()) {
        return maybeModel->second;
    }

    auto inserted = _cache.insert(std::make_pair(lcResRef, doGet(lcResRef)));
    return inserted.first->second;
}

std::shared_ptr<Model> Models::doGet(const std::string &resRef) {
    debug("Load model " + resRef, LogChannel::Graphics);

    auto mdlRes = _resources.find(ResourceId(resRef, ResourceType::Mdl));
    auto mdxRes = _resources.find(ResourceId(resRef, ResourceType::Mdx));
    std::shared_ptr<Model> model;

    if (mdlRes && mdxRes) {
        auto mdl = MemoryInputStream(mdlRes->data);
        auto mdx = MemoryInputStream(mdxRes->data);
        auto reader = MdlMdxReader(mdl, mdx, *this, _textures);
        try {
            reader.load();
            model = reader.model();
        } catch (const FormatException &e) {
            error(boost::format("Error loading model %s: %s") % resRef % std::string(e.what()), LogChannel::Graphics);
        }
    }

    return model;
}

} // namespace graphics

} // namespace reone
