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

#include "../../scene/node/model.h"

namespace reone {

namespace scene {

class MockModelSceneNode : public ModelSceneNode {
public:
    MockModelSceneNode(IUser *user) :
        ModelSceneNode(
            *static_cast<graphics::Model *>(nullptr),
            ModelUsage::Creature,
            *static_cast<SceneGraph *>(nullptr),
            *static_cast<graphics::GraphicsServices *>(nullptr),
            *static_cast<audio::AudioServices *>(nullptr)) {

        _user = user;
    }
};

std::unique_ptr<MockModelSceneNode> mockModelSceneNode(IUser *user = nullptr) {
    return std::make_unique<MockModelSceneNode>(user);
}

} // namespace scene

} // namespace reone
