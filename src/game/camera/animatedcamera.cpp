/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "animatedcamera.h"

#include <unordered_map>

#include <boost/format.hpp>

#include "../../resource/resources.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace game {

AnimatedCamera::AnimatedCamera(SceneGraph *sceneGraph, const CameraStyle &style, float aspect, float zNear, float zFar) {
    _sceneGraph = sceneGraph;

    glm::mat4 projection(glm::perspective(glm::radians(style.viewAngle), aspect, zNear, zFar));
    _sceneNode = make_shared<CameraSceneNode>(_sceneGraph, projection);
}

void AnimatedCamera::update(float dt) {
    if (_model) {
        _model->update(dt);
    }
}

static const std::string &getAnimationName(int animNumber) {
    static map<int, string> nameByNumber;

    auto maybeName = nameByNumber.find(animNumber);
    if (maybeName != nameByNumber.end()) {
        return maybeName->second;
    }
    string name(str(boost::format("CUT%03dW") % (animNumber - 1200 + 1)));

    return nameByNumber.insert(make_pair(animNumber, move(name))).first->second;
}

void AnimatedCamera::playAnimation(int animNumber) {
    if (_model) {
        _model->playAnimation(getAnimationName(animNumber));
    }
}

void AnimatedCamera::setModel(const string &resRef) {
    if (_modelResRef == resRef) return;

    _modelResRef = resRef;
    _model = make_unique<ModelSceneNode>(_sceneGraph, Resources.findModel(resRef));
    _model->attach("camerahook", _sceneNode);
}

} // namespace game

} // namespace reone
