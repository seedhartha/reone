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

#include "reone/graphics/models.h"

#include "reone/common/exception/validation.h"
#include "reone/common/logutil.h"
#include "reone/common/stream/bytearrayinput.h"
#include "reone/resource/resources.h"

#include "reone/graphics/format/mdlreader.h"
#include "reone/graphics/model.h"
#include "reone/graphics/textures.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

Models::Models(Textures &textures, Resources &resources) :
    _textures(textures), _resources(resources) {
}

void Models::invalidate() {
    _cache.clear();
}

shared_ptr<Model> Models::get(const string &resRef) {
    if (resRef.empty()) {
        return nullptr;
    }

    auto lcResRef = boost::to_lower_copy(resRef);
    auto maybeModel = _cache.find(lcResRef);
    if (maybeModel != _cache.end()) {
        return maybeModel->second;
    }

    auto inserted = _cache.insert(make_pair(lcResRef, doGet(lcResRef)));
    return inserted.first->second;
}

shared_ptr<Model> Models::doGet(const string &resRef) {
    debug("Load model " + resRef, LogChannels::graphics);

    shared_ptr<ByteArray> mdlData(_resources.get(resRef, ResourceType::Mdl));
    shared_ptr<ByteArray> mdxData(_resources.get(resRef, ResourceType::Mdx));
    shared_ptr<Model> model;

    if (mdlData && mdxData) {
        auto mdl = ByteArrayInputStream(*mdlData);
        auto mdx = ByteArrayInputStream(*mdxData);
        auto reader = MdlReader(*this, _textures);
        try {
            reader.load(mdl, mdx);
            model = reader.model();
            if (model) {
                model->init();
            }
        } catch (const ValidationException &e) {
            error(boost::format("Error loading model %s: %s") % resRef % string(e.what()), LogChannels::graphics);
        }
    }

    return move(model);
}

} // namespace graphics

} // namespace reone
