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

#include "reone/game/selectioncontroller.h"

#include "reone/common/logutil.h"
#include "reone/scene/graph.h"

#include "reone/game/gameinterface.h"
#include "reone/game/gui/maininterface.h"
#include "reone/game/object.h"
#include "reone/game/object/creature.h"

using namespace std;

namespace reone {

namespace game {

bool SelectionController::handle(const SDL_Event &e) {
    if (e.type == SDL_MOUSEMOTION) {
        auto hoveredSceneNode = _sceneGraph.pickModelAt(e.motion.x, e.motion.y, _pc);
        if (hoveredSceneNode) {
            _hoveredObject = static_cast<Object *>(hoveredSceneNode->user());
            // debug("Mouse hovered at object: " + to_string(_hoveredObject->id()) + "[" + _hoveredObject->tag() + "]");
        } else {
            _hoveredObject = nullptr;
        }
        _mainInterface.setHoveredTarget(_hoveredObject);
        if (_hoveredObject) {
            _game.changeCursor(CursorType::Target);
        } else {
            _game.changeCursor(CursorType::Default);
        }
        return true;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (_hoveredObject) {
            if (_selectedObject == _hoveredObject) {
                _selectedObject->handleClick(*_pc);
            } else {
                _selectedObject = _hoveredObject;
                debug("Object selected: " + to_string(_selectedObject->id()) + "[" + _selectedObject->tag() + "]");
            }
        } else {
            _selectedObject = nullptr;
            debug("Object selection reset");
        }
        _mainInterface.setSelectedTarget(_selectedObject);
        return true;
    }
    return false;
}

} // namespace game

} // namespace reone
