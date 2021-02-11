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

#include "animatedcamera.h"

#include <unordered_map>

#include <boost/format.hpp>

#include "../../render/model/models.h"
#include "../../resource/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

AnimatedCamera::AnimatedCamera(SceneGraph *sceneGraph, float aspect) : _sceneGraph(sceneGraph), _aspect(aspect) {
    _sceneGraph = sceneGraph;
    _sceneNode = make_shared<CameraSceneNode>(_sceneGraph, glm::mat4(1.0f), _zFar);
    updateProjection();
}

void AnimatedCamera::updateProjection() {
    glm::mat4 projection(glm::perspective(glm::radians(_fovy), _aspect, _zNear, _zFar));
    _sceneNode->setProjection(projection);
}

void AnimatedCamera::update(float dt) {
    if (_model) {
        _model->update(dt);
    }
}

static const string &getAnimationName(int animNumber) {
    static map<int, string> nameByNumber;

    auto maybeName = nameByNumber.find(animNumber);
    if (maybeName != nameByNumber.end()) {
        return maybeName->second;
    }
    string name(str(boost::format("cut%03dw") % (animNumber - 1200 + 1)));

    return nameByNumber.insert(make_pair(animNumber, move(name))).first->second;
}

void AnimatedCamera::playAnimation(int animNumber) {
    if (_model) {
        _model->animator().playAnimation(getAnimationName(animNumber));
    }
}

bool AnimatedCamera::isAnimationFinished() const {
    return _model ? _model->animator().isAnimationFinished() : false;
}

void AnimatedCamera::setModel(const shared_ptr<Model> &model) {
    if ((_model && _model->model() == model) ||
        (!_model && !model)) return;

    if (model) {
        _model = make_unique<ModelSceneNode>(_sceneGraph, model);
        _model->attach("camerahook", _sceneNode);
    } else {
        _model.reset();
    }
}

void AnimatedCamera::setFieldOfView(float fovy) {
    _fovy = fovy;
    updateProjection();
}

} // namespace game

} // namespace reone
