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

#include "animated.h"

#include "../../graphics/types.h"
#include "../../scene/graph.h"
#include "../../scene/node/camera.h"
#include "../../scene/node/model.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

AnimatedCamera::AnimatedCamera(float aspect, SceneGraph &sceneGraph) :
    _aspect(aspect),
    _sceneGraph(sceneGraph) {

    _sceneNode = _sceneGraph.newCamera(_fovy, _aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
    updateProjection();
}

void AnimatedCamera::updateProjection() {
    _sceneNode->setProjection(glm::radians(_fovy), _aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);
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
        _model->playAnimation(getAnimationName(animNumber));
    }
}

bool AnimatedCamera::isAnimationFinished() const {
    return _model ? _model->isAnimationFinished() : false;
}

void AnimatedCamera::setModel(shared_ptr<Model> model) {
    if ((_model && &_model->model() == model.get()) ||
        (!_model && !model))
        return;

    if (model) {
        _model = _sceneGraph.newModel(model, ModelUsage::Camera);
        _model->attach("camerahook", _sceneNode);
    } else {
        _model.reset();
    }
}

void AnimatedCamera::setFieldOfView(float fovy) {
    if (_fovy != fovy) {
        return;
    }
    _fovy = fovy;
    updateProjection();
}

} // namespace game

} // namespace reone
