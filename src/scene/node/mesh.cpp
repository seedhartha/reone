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

#include "mesh.h"

#include "../../common/logutil.h"
#include "../../common/randomutil.h"
#include "../../graphics/context.h"
#include "../../graphics/lumautil.h"
#include "../../graphics/mesh.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"
#include "../../graphics/textureutil.h"

#include "../graph.h"

#include "camera.h"
#include "light.h"
#include "model.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kUvAnimationSpeed = 250.0f;

void MeshSceneNode::init() {
    _point = false;
    _alpha = _modelNode->alpha().getByFrameOrElse(0, 1.0f);
    _selfIllumColor = _modelNode->selfIllumColor().getByFrameOrElse(0, glm::vec3(0.0f));

    initTextures();
}

void MeshSceneNode::initTextures() {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return;
    }
    _nodeTextures.diffuse = mesh->diffuseMap;
    _nodeTextures.lightmap = mesh->lightmap;
    _nodeTextures.bumpmap = mesh->bumpmap;

    refreshAdditionalTextures();
}

void MeshSceneNode::refreshAdditionalTextures() {
    _nodeTextures.bumpmap.reset();
    if (!_nodeTextures.diffuse) {
        return;
    }
    const Texture::Features &features = _nodeTextures.diffuse->features();
    if (!features.envmapTexture.empty()) {
        _nodeTextures.envmap = _textures.get(features.envmapTexture, TextureUsage::EnvironmentMap);
    } else if (!features.bumpyShinyTexture.empty()) {
        _nodeTextures.envmap = _textures.get(features.bumpyShinyTexture, TextureUsage::EnvironmentMap);
    }
    if (!features.bumpmapTexture.empty()) {
        _nodeTextures.bumpmap = _textures.get(features.bumpmapTexture, TextureUsage::BumpMap);
    }
}

void MeshSceneNode::update(float dt) {
    SceneNode::update(dt);

    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (mesh) {
        updateUVAnimation(dt, *mesh);
        updateBumpmapAnimation(dt, *mesh);
    }
}

void MeshSceneNode::updateUVAnimation(float dt, const ModelNode::TriangleMesh &mesh) {
    if (mesh.uvAnimation.dir.x != 0.0f || mesh.uvAnimation.dir.y != 0.0f) {
        _uvOffset += kUvAnimationSpeed * mesh.uvAnimation.dir * dt;
        _uvOffset -= glm::floor(_uvOffset);
    }
}

void MeshSceneNode::updateBumpmapAnimation(float dt, const ModelNode::TriangleMesh &mesh) {
    if (!_nodeTextures.bumpmap) {
        return;
    }
    const Texture::Features &features = _nodeTextures.bumpmap->features();
    if (features.procedureType == Texture::ProcedureType::Cycle) {
        int frameCount = features.numX * features.numY;
        float length = frameCount / static_cast<float>(features.fps);
        _bumpmapCycleTime = glm::min(_bumpmapCycleTime + dt, length);
        _bumpmapCycleFrame = static_cast<int>(glm::round((frameCount - 1) * (_bumpmapCycleTime / length)));
        if (_bumpmapCycleTime == length) {
            _bumpmapCycleTime = 0.0f;
        }
    }
}

bool MeshSceneNode::shouldRender() const {
    auto mesh = _modelNode->mesh();
    if (!mesh || !mesh->render || _modelNode->alpha().getByFrameOrElse(0, 1.0f) == 0.0f) {
        return false;
    }
    return !_modelNode->isAABBMesh() && mesh->diffuseMap;
}

bool MeshSceneNode::shouldCastShadows() const {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return false;
    }
    if (_model.usage() == ModelUsage::Creature) {
        return mesh->shadow && !_modelNode->isSkinMesh();
    } else if (_model.usage() == ModelUsage::Placeable) {
        return mesh->render;
    } else {
        return false;
    }
}

bool MeshSceneNode::isTransparent() const {
    if (!_nodeTextures.diffuse) {
        return false;
    }
    auto blending = _nodeTextures.diffuse->features().blending;
    switch (blending) {
    case Texture::Blending::Additive:
        return true;
    case Texture::Blending::PunchThrough:
        return false;
    default:
        break;
    }
    if (_alpha < 1.0f) {
        return true;
    }
    if (_nodeTextures.envmap || _nodeTextures.bumpmap) {
        return false;
    }
    if ((1.0f - rgbToLuma(_selfIllumColor)) < 0.01f) {
        return false;
    }
    return hasAlphaChannel(_nodeTextures.diffuse->pixelFormat());
}

static bool isLightingEnabledByUsage(ModelUsage usage) {
    return usage != ModelUsage::Projectile;
}

bool MeshSceneNode::isSelfIlluminated() const {
    return !_nodeTextures.lightmap && glm::dot(_selfIllumColor, _selfIllumColor) > 0.0f;
}

static bool isReceivingShadows(const ModelSceneNode &model, const MeshSceneNode &modelNode) {
    // Only room models receive shadows, unless model node is self-illuminated
    return model.usage() == ModelUsage::Room && !modelNode.isSelfIlluminated();
}

void MeshSceneNode::draw() {
    auto mesh = _modelNode->mesh();
    if (!mesh || !_nodeTextures.diffuse) {
        return;
    }
    bool transparent = isTransparent();

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.model = _absTransform;
    uniforms.general.modelInv = _absTransformInv;
    uniforms.general.alpha = _alpha;
    uniforms.general.uv = glm::mat3x4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(_uvOffset.x, _uvOffset.y, 0.0f, 0.0f));

    _textures.bind(*_nodeTextures.diffuse);
    switch (_nodeTextures.diffuse->features().blending) {
    case Texture::Blending::PunchThrough:
        uniforms.general.featureMask |= UniformsFeatureFlags::hashedalphatest;
        break;
    case Texture::Blending::Additive:
        if (!_nodeTextures.envmap) {
            uniforms.general.featureMask |= UniformsFeatureFlags::premulalpha;
        }
        break;
    default:
        break;
    }
    float waterAlpha = _nodeTextures.diffuse->features().waterAlpha;
    if (waterAlpha != -1.0f) {
        uniforms.general.featureMask |= UniformsFeatureFlags::water;
        uniforms.general.waterAlpha = waterAlpha;
    }

    if (_nodeTextures.lightmap) {
        uniforms.general.featureMask |= UniformsFeatureFlags::lightmap;
        _textures.bind(*_nodeTextures.lightmap, TextureUnits::lightmap);
    }
    if (_nodeTextures.envmap) {
        uniforms.general.featureMask |= UniformsFeatureFlags::envmap;
        if (_nodeTextures.envmap->isCubemap()) {
            uniforms.general.featureMask |= UniformsFeatureFlags::envmapcube;
            _textures.bind(*_nodeTextures.envmap, TextureUnits::environmentMapCube);
        } else {
            _textures.bind(*_nodeTextures.envmap, TextureUnits::environmentMap);
        }
    }
    if (_nodeTextures.bumpmap) {
        if (_nodeTextures.bumpmap->isGrayscale()) {
            uniforms.general.featureMask |= UniformsFeatureFlags::heightmap;
            uniforms.general.heightMapScaling = _nodeTextures.bumpmap->features().bumpMapScaling;
            if (_nodeTextures.bumpmap->features().procedureType == Texture::ProcedureType::Cycle) {
                int gridX = _nodeTextures.bumpmap->features().numX;
                int gridY = _nodeTextures.bumpmap->features().numY;
                glm::vec2 oneOverGrid(1.0f / glm::vec2(static_cast<float>(gridX), static_cast<float>(gridY)));
                uniforms.general.heightMapFrameBounds = glm::vec4(
                    oneOverGrid.x * (_bumpmapCycleFrame % gridX),
                    oneOverGrid.y * (_bumpmapCycleFrame / gridX),
                    oneOverGrid.x,
                    oneOverGrid.y);
            } else {
                uniforms.general.heightMapFrameBounds = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }
        } else {
            uniforms.general.featureMask |= UniformsFeatureFlags::normalmap;
        }
        _textures.bind(*_nodeTextures.bumpmap, TextureUnits::bumpMap);
    }
    auto skin = mesh->skin;
    if (skin) {
        uniforms.general.featureMask |= UniformsFeatureFlags::skeletal;
        for (size_t i = 0; i < kMaxBones; ++i) {
            if (i >= skin->boneNodeNumber.size()) {
                break;
            }
            auto nodeNumber = skin->boneNodeNumber[i];
            if (nodeNumber == 0xffff) {
                continue;
            }
            auto bone = _model.getNodeByNumber(nodeNumber);
            if (!bone) {
                continue;
            }
            uniforms.skeletal.bones[i] = _modelNode->absoluteTransformInverse() *
                                         _model.absoluteTransformInverse() *
                                         bone->absoluteTransform() *
                                         skin->boneMatrices[skin->boneSerial[i]];
        }
    }
    bool receivesShadows = isReceivingShadows(_model, *this);
    if (receivesShadows && _sceneGraph.hasShadowLight()) {
        uniforms.general.featureMask |= UniformsFeatureFlags::shadows;
    }
    if (isSelfIlluminated()) {
        uniforms.general.featureMask |= UniformsFeatureFlags::selfillum;
        uniforms.general.selfIllumColor = glm::vec4(_selfIllumColor, 1.0f);
    }
    if (_sceneGraph.isFogEnabled() && _model.model().isAffectedByFog()) {
        uniforms.general.featureMask |= UniformsFeatureFlags::fog;
    }
    auto &program = transparent ? _shaders.modelTransparent() : _shaders.modelOpaque();
    _shaders.use(program, true);
    _graphicsContext.withFaceCulling(CullFaceMode::Back, [&mesh]() {
        mesh->mesh->draw();
    });
}

void MeshSceneNode::drawShadow() {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return;
    }
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.model = _absTransform;
    uniforms.general.modelInv = _absTransformInv;
    uniforms.general.alpha = _alpha;

    auto &program = _sceneGraph.isShadowLightDirectional() ? _shaders.directionalLightShadows() : _shaders.pointLightShadows();
    _shaders.use(program, true);
    mesh->mesh->draw();
}

bool MeshSceneNode::isLightingEnabled() const {
    if (!isLightingEnabledByUsage(_model.usage())) {
        return false;
    }
    // Lighting is disabled for self-illuminated model nodes, e.g. sky boxes
    if (isSelfIlluminated()) {
        return false;
    }
    // Lighting is disabled when diffuse texture is additive
    if (_nodeTextures.diffuse && _nodeTextures.diffuse->features().blending == Texture::Blending::Additive) {
        return false;
    }
    return true;
}

void MeshSceneNode::setDiffuseMap(shared_ptr<Texture> texture) {
    ModelNodeSceneNode::setDiffuseMap(texture);
    _nodeTextures.diffuse = texture;
    refreshAdditionalTextures();
}

void MeshSceneNode::setEnvironmentMap(shared_ptr<Texture> texture) {
    ModelNodeSceneNode::setEnvironmentMap(texture);
    _nodeTextures.envmap = move(texture);
}

} // namespace scene

} // namespace reone
