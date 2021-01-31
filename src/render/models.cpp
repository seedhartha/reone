/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "model/gr2file.h"
#include "model/mdlfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

Models &Models::instance() {
    static Models instance;
    return instance;
}

void Models::init(GameID gameId) {
    _gameId = gameId;
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
    shared_ptr<Model> model;

    // Try GR2 models first. This is a hook to get SWTOR models working.
    shared_ptr<ByteArray> gr2Data(Resources::instance().get(resRef, ResourceType::Gr2));
    if (gr2Data) {
        Gr2File gr2;
        gr2.load(wrap(gr2Data));
        model = gr2.model();

    } else {
        shared_ptr<ByteArray> mdlData(Resources::instance().get(resRef, ResourceType::Mdl));
        shared_ptr<ByteArray> mdxData(Resources::instance().get(resRef, ResourceType::Mdx));
        if (mdlData && mdxData) {
            MdlFile mdl(_gameId);
            mdl.load(wrap(mdlData), wrap(mdxData));
            model = mdl.model();
        }
    }
    if (model) {
        model->initGL();
    }

    return move(model);
}

} // namespace render

} // namespace reone
