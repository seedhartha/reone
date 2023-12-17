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

#pragma once

#include "reone/graphics/model.h"
#include "reone/resource/id.h"
#include "reone/scene/node/camera.h"
#include "reone/scene/node/model.h"
#include "reone/system/stream/input.h"

#include "../../property.h"
#include "../../viewmodel.h"

namespace reone {

namespace graphics {

class GraphicsModule;

}

namespace resource {

class ResourceModule;

}

namespace scene {

class SceneModule;

}

class SystemModule;

struct AnimationProgress {
    float time {0.0f};
    float duration {0.0f};
};

class ModelResourceViewModel : public ViewModel {
public:
    ModelResourceViewModel(SystemModule &systemSvc,
                           graphics::GraphicsModule &graphicsSvc,
                           resource::ResourceModule &resourceSvc,
                           scene::SceneModule &sceneSvc) :
        _systemSvc(systemSvc),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc),
        _sceneSvc(sceneSvc) {
    }

    Property<std::vector<std::string>> &animations() { return _animations; }
    Property<AnimationProgress> &animationProgress() { return _animationProgress; }

    void initScene();

    void openModel(const resource::ResourceId &id, IInputStream &mdl);

    void update3D();
    void render3D(int w, int h);

    void playAnimation(std::string anim, graphics::LipAnimation *lipAnim = nullptr);
    void pauseAnimation();
    void resumeAnimation();
    void setAnimationTime(float time);

    bool isAnimationPlaying() const { return _animationPlaying; }

    void onGLCanvasMouseMotion(int x, int y, bool leftDown, bool rightDown);
    void onGLCanvasMouseWheel(int delta);

private:
    SystemModule &_systemSvc;
    graphics::GraphicsModule &_graphicsSvc;
    resource::ResourceModule &_resourceSvc;
    scene::SceneModule &_sceneSvc;

    Property<std::vector<std::string>> _animations;
    Property<AnimationProgress> _animationProgress;

    std::shared_ptr<scene::CameraSceneNode> _cameraNode;
    std::shared_ptr<graphics::Model> _model;
    std::shared_ptr<scene::ModelSceneNode> _modelNode;
    glm::vec3 _cameraPosition {0.0f};
    float _modelHeading {0.0f};
    float _modelPitch {0.0f};
    int _lastMouseX {0};
    int _lastMouseY {0};
    uint32_t _lastTicks {0};
    bool _animationPlaying {false};

    void updateModelTransform();
    void updateCameraTransform();
};

} // namespace reone
