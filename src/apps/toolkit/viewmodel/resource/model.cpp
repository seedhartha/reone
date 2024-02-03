/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "model.h"

#include "reone/game/types.h"
#include "reone/graphics/animation.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/module.h"
#include "reone/graphics/format/mdlmdxreader.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/module.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/resources.h"
#include "reone/scene/di/module.h"
#include "reone/scene/graphs.h"
#include "reone/system/clock.h"
#include "reone/system/di/module.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::game;
using namespace reone::graphics;
using namespace reone::scene;
using namespace reone::resource;

namespace reone {

void ModelResourceViewModel::initScene() {
    auto &graphs = _sceneSvc.graphs();
    graphs.reserve(kSceneMain);
    auto &scene = graphs.get(kSceneMain);
    _cameraNode = scene.newCamera();
    scene.setActiveCamera(_cameraNode.get());
    scene.setAmbientLightColor(glm::vec3(1.0f));
}

void ModelResourceViewModel::openModel(const ResourceId &id, IInputStream &mdl) {
    auto mdxRes = _resourceSvc.resources().find(ResourceId(id.resRef, ResType::Mdx));
    if (!mdxRes) {
        throw ResourceNotFoundException("Companion MDX resource not found: " + id.resRef.value());
    }
    auto mdx = MemoryInputStream(mdxRes->data);
    auto reader = MdlMdxReader(mdl, mdx, _graphicsModule.statistic());
    reader.load();

    auto &scene = _sceneSvc.graphs().get(kSceneMain);
    scene.clear();
    scene.update(0.0f);
    _modelNode.reset();

    _model = reader.model();
    if (!_model->superModelName().empty()) {
        auto superModel = _resourceSvc.models().get(_model->superModelName());
        _model->setSuperModel(std::move(superModel));
    }
    _model->init();
    _animations = _model->getAnimationNames();

    _modelNode = scene.newModel(*_model, ModelUsage::Creature);
    _modelHeading = 0.0f;
    _modelPitch = 0.0f;
    updateModelTransform();
    scene.addRoot(_modelNode);

    _cameraPosition = glm::vec3(0.0f, 8.0f, 0.0f);
    updateCameraTransform();
}

void ModelResourceViewModel::update3D() {
    auto ticks = _systemSvc.clock().millis();
    if (_lastTicks == 0) {
        _lastTicks = ticks;
    }
    float delta = (ticks - _lastTicks) / 1000.0f;
    _lastTicks = ticks;

    auto &scene = _sceneSvc.graphs().get(kSceneMain);
    scene.update(delta);

    AnimationProgress progress;
    if (_modelNode && !_modelNode->animationChannels().empty()) {
        const auto &animChannel = _modelNode->animationChannels().front();
        if (animChannel.anim) {
            progress.playing = true;
            progress.time = animChannel.time;
            progress.duration = animChannel.lipAnim ? animChannel.lipAnim->length() : animChannel.anim->length();
        }
    }
    _animationProgress = std::move(progress);
}

void ModelResourceViewModel::render3D(int w, int h) {
    float aspect = w / static_cast<float>(h);
    _cameraNode->setPerspectiveProjection(glm::radians(55.0f), aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);

    auto &scene = _sceneSvc.graphs().get(kSceneMain);
    auto &output = scene.render(glm::ivec2(w, h));
    _graphicsModule.uniforms().setLocals(std::bind(&LocalUniforms::reset, std::placeholders::_1));
    _graphicsModule.context().useProgram(_graphicsModule.shaderRegistry().get(ShaderProgramId::ndcTexture));
    _graphicsModule.context().bindTexture(output);
    _graphicsModule.context().withViewport(glm::ivec4(0, 0, w, h), [this, &output]() {
        _graphicsModule.context().clearColorDepth();
        _graphicsModule.meshRegistry().get(MeshName::quadNDC).draw(_graphicsModule.statistic());
    });
}

void ModelResourceViewModel::playAnimation(std::string anim, graphics::LipAnimation *lipAnim) {
    if (!_modelNode) {
        return;
    }
    _modelNode->playAnimation(anim, lipAnim, AnimationProperties::fromFlags(AnimationFlags::loop));
    _animationPlaying = true;
}

void ModelResourceViewModel::pauseAnimation() {
    if (!_modelNode) {
        return;
    }
    _modelNode->pauseAnimation();
    _animationPlaying = false;
}

void ModelResourceViewModel::resumeAnimation() {
    if (!_modelNode) {
        return;
    }
    _modelNode->resumeAnimation();
    _animationPlaying = true;
}

void ModelResourceViewModel::setAnimationTime(float time) {
    if (!_modelNode) {
        return;
    }
    _modelNode->setAnimationTime(time);
}

void ModelResourceViewModel::updateModelTransform() {
    auto transform = glm::mat4(1.0f);
    transform *= glm::rotate(_modelHeading, glm::vec3(0.0f, 0.0f, 1.0f));
    transform *= glm::rotate(_modelPitch, glm::vec3(-1.0f, 0.0f, 0.0f));
    _modelNode->setLocalTransform(transform);
}

void ModelResourceViewModel::updateCameraTransform() {
    auto cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, _cameraPosition);
    cameraTransform *= glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraTransform *= glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    _cameraNode->setLocalTransform(cameraTransform);
}

void ModelResourceViewModel::onGLCanvasMouseMotion(int x, int y, bool leftDown, bool rightDown) {
    int dx = x - _lastMouseX;
    int dy = y - _lastMouseY;

    if (leftDown) {
        _modelHeading += dx / glm::pi<float>() / 64.0f;
        //_modelPitch += dy / glm::pi<float>() / 64.0f;
        updateModelTransform();
    } else if (rightDown) {
        _cameraPosition.x += dx / static_cast<float>(256.0f);
        _cameraPosition.z += dy / static_cast<float>(256.0f);
        updateCameraTransform();
    }

    _lastMouseX = x;
    _lastMouseY = y;
}

void ModelResourceViewModel::onGLCanvasMouseWheel(int delta) {
    _cameraPosition.y = glm::max(0.0f, _cameraPosition.y - glm::clamp(delta, -1, 1));
    updateCameraTransform();
}

} // namespace reone
