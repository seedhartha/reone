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

#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../resource/resources.h"

#include "../model/mdlreader.h"

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

void Models::registerLoader(ResourceType type, shared_ptr<IModelLoader> loader) {
    _loaders.insert(make_pair(type, move(loader)));
}

shared_ptr<Model> Models::get(const string &resRef, ResourceType type) {
    if (resRef.empty()) return nullptr;

    auto maybeModel = _cache.find(resRef);
    if (maybeModel != _cache.end()) return maybeModel->second;

    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef, type)));
    return inserted.first->second;
}

shared_ptr<Model> Models::doGet(const string &resRef, ResourceType type) {
    auto maybeLoader = _loaders.find(type);
    if (maybeLoader == _loaders.end()) {
        warn("Model loader not found by ResType: " + to_string(static_cast<int>(type)));
        return nullptr;
    }

    debug("Load model " + resRef);
    shared_ptr<Model> model(maybeLoader->second->loadModel(_gameId, resRef));
    if (model) {
        model->initGL();
    }

    return move(model);
}

} // namespace render

} // namespace reone
