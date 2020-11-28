/*
 * Copyright (c) 2020 The reone project contributors
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

#include "models.h"

#include "../common/streamutil.h"
#include "../resource/resources.h"

#include "model/mdlfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

Models &Models::instance() {
    static Models instance;
    return instance;
}

void Models::init(GameVersion version) {
    _version = version;
}

void Models::invalidateCache() {
    _cache.clear();
}

shared_ptr<Model> Models::get(const string &resRef) {
    auto maybeModel = _cache.find(resRef);
    if (maybeModel != _cache.end()) {
        return maybeModel->second;
    }
    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef)));

    return inserted.first->second;
}

shared_ptr<Model> Models::doGet(const string &resRef) {
    shared_ptr<ByteArray> mdlData(Resources::instance().get(resRef, ResourceType::Model));
    shared_ptr<ByteArray> mdxData(Resources::instance().get(resRef, ResourceType::Mdx));
    shared_ptr<Model> model;

    if (mdlData && mdxData) {
        MdlFile mdl(_version);
        mdl.load(wrap(mdlData), wrap(mdxData));
        model = mdl.model();
        if (model) {
            model->initGL();
        }
    }

    return move(model);
}

} // namespace render

} // namespace reone
