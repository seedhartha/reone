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

#include "area.h"

#include "glm/vec3.hpp"

#include "../game.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

void Area::updateObjectSelection() {
    if (_hilightedObject && !_hilightedObject->isSelectable()) {
        _hilightedObject.reset();
    }
    if (_selectedObject && !_selectedObject->isSelectable()) {
        _selectedObject.reset();
    }
}

void Area::selectNextObject(bool reverse) {
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

vector<shared_ptr<SpatialObject>> Area::getSelectableObjects() const {
    vector<shared_ptr<SpatialObject>> result;
    vector<pair<shared_ptr<SpatialObject>, float>> distances;

    shared_ptr<SpatialObject> partyLeader(_game->party().getLeader());
    glm::vec3 origin(partyLeader->position());

    for (auto &object : objects()) {
        if (!object->isSelectable() || object.get() == partyLeader.get()) continue;

        auto model = static_pointer_cast<ModelSceneNode>(object->sceneNode());
        if (!model || !model->isVisible()) continue;

        float dist2 = object->getDistanceTo2(origin);
        if (dist2 > kSelectionDistance * kSelectionDistance) continue;

        distances.push_back(make_pair(object, dist2));
    }

    sort(distances.begin(), distances.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    for (auto &pair : distances) {
        result.push_back(pair.first);
    }

    return move(result);
}

void Area::selectNearestObject() {
    _selectedObject.reset();
    selectNextObject();
}

void Area::hilightObject(shared_ptr<SpatialObject> object) {
    _hilightedObject = move(object);
}

void Area::selectObject(shared_ptr<SpatialObject> object) {
    _selectedObject = move(object);
}

} // namespace game

} // namespace reone
