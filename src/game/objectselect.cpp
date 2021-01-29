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

#include "objectselect.h"

#include <stdexcept>

#include "glm/vec3.hpp"

#include "object/area.h"
#include "party.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

ObjectSelector::ObjectSelector(const Area *area, const Party *party) :
    _area(area), _party(party) {

    if (!area) {
        throw invalid_argument("area must not be null");
    }
    if (!party) {
        throw invalid_argument("party must not be null");
    }
}

void ObjectSelector::update() {
    if (_hilightedObject && !_hilightedObject->isSelectable()) {
        _hilightedObject.reset();
    }
    if (_selectedObject && !_selectedObject->isSelectable()) {
        _selectedObject.reset();
    }
}

void ObjectSelector::selectNext(bool reverse) {
    vector<shared_ptr<SpatialObject>> selectables(getSelectableObjects());

    if (selectables.empty()) {
        _selectedObject.reset();
        return;
    }
    if (!_selectedObject) {
        _selectedObject = selectables.front();
        return;
    }
    if (reverse) {
        auto selected = std::find(selectables.rbegin(), selectables.rend(), _selectedObject);
        if (selected != selectables.rend()) {
            selected++;
        }
        _selectedObject = selected != selectables.rend() ? *selected : selectables.back();

    } else {
        auto selected = std::find(selectables.begin(), selectables.end(), _selectedObject);
        if (selected != selectables.end()) {
            selected++;
        }
        _selectedObject = selected != selectables.end() ? *selected : selectables.front();
    }
}

vector<shared_ptr<SpatialObject>> ObjectSelector::getSelectableObjects() const {
    vector<shared_ptr<SpatialObject>> result;
    vector<pair<shared_ptr<SpatialObject>, float>> distances;

    shared_ptr<SpatialObject> partyLeader(_party->getLeader());
    glm::vec3 origin(partyLeader->position());

    for (auto &object : _area->objects()) {
        if (!object->isSelectable() || object.get() == partyLeader.get()) continue;

        shared_ptr<ModelSceneNode> model(object->model());
        if (!model || !model->isVisible()) continue;

        float dist = object->distanceTo(origin);
        if (dist > kSelectionDistance) continue;

        distances.push_back(make_pair(object, dist));
    }

    sort(distances.begin(), distances.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    for (auto &pair : distances) {
        result.push_back(pair.first);
    }

    return move(result);
}

void ObjectSelector::selectNearest() {
    _selectedObject.reset();
    selectNext();
}

void ObjectSelector::hilight(const shared_ptr<SpatialObject> &object) {
    _hilightedObject = object;
}

void ObjectSelector::select(const shared_ptr<SpatialObject> &object) {
    _selectedObject = object;
}

} // namespace game

} // namespace reone
