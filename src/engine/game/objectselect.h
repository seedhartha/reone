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

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

namespace reone {

namespace game {

class Area;
class SpatialObject;
class Party;

class ObjectSelector : boost::noncopyable {
public:
    ObjectSelector(const Area *area, const Party *party);

    void update();
    void selectNext(bool reverse = false);
    void selectNearest();
    void hilight(const std::shared_ptr<SpatialObject> &object);
    void select(const std::shared_ptr<SpatialObject> &object);

    std::vector<std::shared_ptr<SpatialObject>> getSelectableObjects() const;

    std::shared_ptr<SpatialObject> hilightedObject() const { return _hilightedObject; }
    std::shared_ptr<SpatialObject> selectedObject() const { return _selectedObject; }

private:
    const Area *_area { nullptr };
    const Party *_party { nullptr };
    std::shared_ptr<SpatialObject> _hilightedObject;
    std::shared_ptr<SpatialObject> _selectedObject;
};

} // namespace game

} // namespace reone
