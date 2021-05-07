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

#include "modelnodescenenode.h"

#include <stdexcept>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "../../common/log.h"
#include "../../common/random.h"
#include "../../graphics/featureutil.h"
#include "../../graphics/materials.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/statemanager.h"
#include "../../graphics/textures.h"

#include "../scenegraph.h"

#include "cameranode.h"
#include "lightnode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kUvAnimationSpeed = 250.0f;

static bool g_debugWalkmesh = false;

ModelNodeSceneNode::ModelNodeSceneNode(SceneGraph *sceneGraph, const ModelSceneNode *modelSceneNode, ModelNode *modelNode) :
    SceneNode(SceneNodeType::ModelNode, sceneGraph),
    _modelSceneNode(modelSceneNode),
    _modelNode(modelNode) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
    if (!modelNode) {
        throw invalid_argument("modelNode must not be null");
    }
    if (_modelNode->alphas().getNumKeyframes() > 0) {
        _alpha = _modelNode->alphas().getByKeyframe(0);
    }
    if (_modelNode->selfIllumColors().getNumKeyframes() > 0) {
        _selfIllumColor = _modelNode->selfIllumColors().getByKeyframe(0);
    }
    initTextures();
}

void ModelNodeSceneNode::initTextures() {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    _textures.diffuse = mesh->diffuseTexture();
    _textures.lightmap = mesh->lightmapTexture();
    _textures.bumpmap = mesh->bumpmapTexture();

    refreshMaterial();
    refreshAdditionalTextures();
}

void ModelNodeSceneNode::refreshMaterial() {
    if (!_textures.diffuse) return;

    shared_ptr<Material> material(Materials::instance().get(_textures.diffuse->name()));
    if (material) {
        _material = *material;
    }
}

void ModelNodeSceneNode::refreshAdditionalTextures() {
    if (!_textures.diffuse) return;

    const Texture::Features &features = _textures.diffuse->features();
    if (!features.envmapTexture.empty()) {
        _textures.envmap = Textures::instance().get(features.envmapTexture, TextureUsage::EnvironmentMap);
    } else if (!features.bumpyShinyTexture.empty()) {
        _textures.envmap = Textures::instance().get(features.bumpyShinyTexture, TextureUsage::EnvironmentMap);
    }
    if (!features.bumpmapTexture.empty()) {
        _textures.bumpmap = Textures::instance().get(features.bumpmapTexture, TextureUsage::Bumpmap);
    }
}

void ModelNodeSceneNode::update(float dt) {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (mesh) {
        // UV animation
        const ModelMesh::UVAnimation &uvAnimation = mesh->uvAnimation();
        if (uvAnimation.animated) {
            glm::vec2 dir(uvAnimation.directionX, uvAnimation.directionY);
            _uvOffset += kUvAnimationSpeed * dir * dt;
            _uvOffset -= glm::floor(_uvOffset);
        }

        // Bumpmap UV animation
        if (_textures.bumpmap) {
            const Texture::Features &features = _textures.bumpmap->features();
            if (features.procedureType == Texture::ProcedureType::Cycle) {
                int frameCount = features.numX * features.numY;
                float length = frameCount / static_cast<float>(features.fps);
                _bumpmapTime = glm::min(_bumpmapTime + dt, length);
                _bumpmapFrame = static_cast<int>(glm::round((frameCount - 1) * (_bumpmapTime / length)));
                if (_bumpmapTime == length) {
                    _bumpmapTime = 0.0f;
                }
            }
        }

        // Danglymesh animation
        shared_ptr<ModelNode::Danglymesh> danglymesh(_modelNode->danglymesh());
        if (danglymesh) {
            bool forceApplied = glm::length2(_danglymeshAnimation.force) > 0.0f;
            if (forceApplied) {
                // When force is applied, stride in the opposite direction from the applied force
                glm::vec3 strideDir(-_danglymeshAnimation.force);
                glm::vec3 maxStride(danglymesh->displacement);
                _danglymeshAnimation.stride = glm::clamp(_danglymeshAnimation.stride + danglymesh->period * strideDir * dt, -maxStride, maxStride);
            } else {
                // When force is not applied, gradually nullify stride
                float strideMag2 = glm::length2(_danglymeshAnimation.stride);
                if (strideMag2 > 0.0f) {
                    glm::vec3 strideDir(-_danglymeshAnimation.stride);
                    _danglymeshAnimation.stride += danglymesh->period * strideDir * dt;
                    if ((strideDir.x > 0.0f && _danglymeshAnimation.stride.x > 0.0f) || (strideDir.x < 0.0f && _danglymeshAnimation.stride.x < 0.0f)) {
                        _danglymeshAnimation.stride.x = 0.0f;
                    }
                    if ((strideDir.y > 0.0f && _danglymeshAnimation.stride.y > 0.0f) || (strideDir.y < 0.0f && _danglymeshAnimation.stride.y < 0.0f)) {
                        _danglymeshAnimation.stride.y = 0.0f;
                    }
                    if ((strideDir.z > 0.0f && _danglymeshAnimation.stride.z > 0.0f) || (strideDir.z < 0.0f && _danglymeshAnimation.stride.z < 0.0f)) {
                        _danglymeshAnimation.stride.z = 0.0f;
                    }
                }
            }
        }
    }

    SceneNode::update(dt);
}

bool ModelNodeSceneNode::shouldRender() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh || !mesh->shouldRender() || _modelNode->alphas().getByKeyframeOrElse(0, 1.0f) == 0.0f) return false;

    return _modelNode->isAABB() ? g_debugWalkmesh : true;
}

bool ModelNodeSceneNode::shouldCastShadows() const {
    if (_modelSceneNode->usage() != ModelUsage::Creature) return false;

    shared_ptr<ModelMesh> mesh(_modelNode->mesh());

    return mesh && mesh->shouldCastShadows() && !static_cast<bool>(_modelNode->skin());
}

bool ModelNodeSceneNode::isTransparent() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return false; // Meshless nodes are opaque

    // Character models are opaque
    if (_modelSceneNode->model()->classification() == Model::Classification::Character) return false;

    // Model nodes with alpha less than 1.0 are transparent
    if (_alpha < 1.0f) return true;

    // Model nodes without a diffuse texture are opaque
    if (!_textures.diffuse) return false;

    // Model nodes with transparency hint greater than 0 are transparent
    if (mesh->transparency() > 0) return true;

    // Model nodes with additive diffuse texture are opaque
    if (_textures.diffuse->isAdditive()) return true;

    // Model nodes with an environment map or a bump map are opaque
    if (_textures.envmap || _textures.bumpmap) return false;

    // Model nodes with RGB diffuse textures are opaque
    PixelFormat format = _textures.diffuse->pixelFormat();
    if (format == PixelFormat::RGB || format == PixelFormat::BGR || format == PixelFormat::DXT1) return false;

    return true;
}

static bool isLightingEnabledByUsage(ModelUsage usage) {
    return usage != ModelUsage::Projectile;
}

bool ModelNodeSceneNode::isSelfIlluminated() const {
    return !_textures.lightmap && glm::dot(_selfIllumColor, _selfIllumColor) > 0.0f;
}

static bool isReceivingShadows(const ModelSceneNode &model, const ModelNodeSceneNode &modelNode) {
    // Only room models receive shadows, unless model node is self-illuminated
    return
        model.usage() == ModelUsage::Room &&
        !modelNode.isSelfIlluminated();
}

void ModelNodeSceneNode::drawSingle(bool shadowPass) {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    // Setup shaders

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    if (isFeatureEnabled(Feature::HDR)) {
        uniforms.combined.featureMask |= UniformFeatureFlags::hdr;
    }
    uniforms.combined.general.model = _absoluteTransform;
    uniforms.combined.general.alpha = _modelSceneNode->alpha() * _alpha;
    uniforms.combined.general.ambientColor = glm::vec4(_sceneGraph->ambientLightColor(), 1.0f);
    uniforms.combined.general.exposure = _sceneGraph->exposure();

    ShaderProgram program;

    if (shadowPass) {
        program = ShaderProgram::SimpleDepth;

    } else {
        if (!_textures.diffuse) {
            program = ShaderProgram::ModelBlinnPhongDiffuseless;
        } else if (isFeatureEnabled(Feature::PBR)) {
            program = ShaderProgram::ModelPBR;
        } else {
            program = ShaderProgram::ModelBlinnPhong;
        }

        if (_textures.diffuse) {
            uniforms.combined.featureMask |= UniformFeatureFlags::diffuse;
        }

        if (_textures.envmap) {
            uniforms.combined.featureMask |= UniformFeatureFlags::envmap;

            if (isFeatureEnabled(Feature::PBR)) {
                bool derived = PBRIBL::instance().contains(_textures.envmap.get());
                if (derived) {
                    uniforms.combined.featureMask |= UniformFeatureFlags::pbrIbl;
                }
            }
        }

        if (_textures.lightmap) {
            uniforms.combined.featureMask |= UniformFeatureFlags::lightmap;
        }

        if (_textures.bumpmap) {
            uniforms.combined.featureMask |= UniformFeatureFlags::bumpmaps;
            uniforms.combined.bumpmaps.grayscale = _textures.bumpmap->isGrayscale();
            uniforms.combined.bumpmaps.scaling = _textures.bumpmap->features().bumpMapScaling;
            uniforms.combined.bumpmaps.gridSize = glm::vec2(_textures.bumpmap->features().numX, _textures.bumpmap->features().numY);
            uniforms.combined.bumpmaps.frame = _bumpmapFrame;
            uniforms.combined.bumpmaps.swizzled = mesh->isBumpmapSwizzled();
        }

        bool receivesShadows = isReceivingShadows(*_modelSceneNode, *this);
        if (receivesShadows) {
            uniforms.combined.featureMask |= UniformFeatureFlags::shadows;
        }

        shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
        if (skin) {
            uniforms.combined.featureMask |= UniformFeatureFlags::skeletal;

            for (int i = 0; i < kMaxBones; ++i) {
                uniforms.skeletal->bones[i] = glm::mat4(1.0f);
            }
            for (auto &pair : skin->nodeIdxByBoneIdx) {
                uint16_t boneIdx = pair.first;
                uint16_t nodeIdx = pair.second;

                ModelNodeSceneNode *bone = _modelSceneNode->getModelNodeByIndex(nodeIdx);
                if (bone) {
                    uniforms.skeletal->bones[boneIdx] = _modelNode->absoluteTransformInverse() * bone->boneTransform() * _modelNode->absoluteTransform();
                }
            }
        }

        if (isSelfIlluminated()) {
            uniforms.combined.featureMask |= UniformFeatureFlags::selfIllum;
            uniforms.combined.general.selfIllumColor = glm::vec4(_selfIllumColor, 1.0f);
        }
        if (isLightingEnabled()) {
            const vector<LightSceneNode *> &lights = _modelSceneNode->lightsAffectedBy();

            uniforms.combined.featureMask |= UniformFeatureFlags::lighting;
            if (_material.custom) {
                uniforms.combined.featureMask |= UniformFeatureFlags::customMat;
            }
            uniforms.combined.material.ambient = glm::vec4(mesh->ambientColor(), 1.0f);
            uniforms.combined.material.diffuse = glm::vec4(mesh->diffuseColor(), 1.0f);
            uniforms.combined.material.specular = _material.specular;
            uniforms.combined.material.shininess = _material.shininess;
            uniforms.combined.material.metallic = _material.metallic;
            uniforms.combined.material.roughness = _material.roughness;
            uniforms.lighting->lightCount = static_cast<int>(lights.size());

            for (int i = 0; i < uniforms.lighting->lightCount; ++i) {
                glm::vec4 position(lights[i]->absoluteTransform()[3]);
                position.w = lights[i]->isDirectional() ? 0.0f : 1.0f;

                ShaderLight &shaderLight = uniforms.lighting->lights[i];
                shaderLight.position = move(position);
                shaderLight.color = glm::vec4(lights[i]->color(), 1.0f);
                shaderLight.multiplier = lights[i]->multiplier();
                shaderLight.radius = lights[i]->radius();
            }
        }

        if (_textures.diffuse) {
            uniforms.combined.general.uvOffset = _uvOffset;

            float waterAlpha = _textures.diffuse->features().waterAlpha;
            if (waterAlpha != -1.0f) {
                uniforms.combined.featureMask |= UniformFeatureFlags::water;
                uniforms.combined.general.waterAlpha = waterAlpha;
            }
        }

        if (_sceneGraph->isFogEnabled() && _modelSceneNode->model()->isAffectedByFog()) {
            uniforms.combined.featureMask |= UniformFeatureFlags::fog;
            uniforms.combined.general.fogNear = _sceneGraph->fogNear();
            uniforms.combined.general.fogFar = _sceneGraph->fogFar();
            uniforms.combined.general.fogColor = glm::vec4(_sceneGraph->fogColor(), 1.0f);
        }

        shared_ptr<ModelNode::Danglymesh> danglymesh(_modelNode->danglymesh());
        if (danglymesh) {
            uniforms.combined.featureMask |= UniformFeatureFlags::danglymesh;
            uniforms.danglymesh->stride = glm::vec4(_danglymeshAnimation.stride, 0.0f);
            uniforms.danglymesh->displacement = danglymesh->displacement;
            size_t i = 0;
            for (i = 0; i < danglymesh->constraints.size(); ++i) {
                uniforms.danglymesh->constraints[i / 4][i % 4] = danglymesh->constraints[i].multiplier;
            }
        }
    }

    Shaders::instance().activate(program, uniforms);


    bool additive = false;

    // Setup textures

    if (_textures.diffuse) {
        StateManager::instance().setActiveTextureUnit(TextureUnits::diffuse);
        _textures.diffuse->bind();
        additive = _textures.diffuse->isAdditive();
    }
    if (_textures.lightmap) {
        StateManager::instance().setActiveTextureUnit(TextureUnits::lightmap);
        _textures.lightmap->bind();
    }
    if (_textures.envmap) {
        StateManager::instance().setActiveTextureUnit(TextureUnits::envmap);
        _textures.envmap->bind();

        PBRIBL::Derived derived;
        if (PBRIBL::instance().getDerived(_textures.envmap.get(), derived)) {
            StateManager::instance().setActiveTextureUnit(TextureUnits::irradianceMap);
            derived.irradianceMap->bind();
            StateManager::instance().setActiveTextureUnit(TextureUnits::prefilterMap);
            derived.prefilterMap->bind();
            StateManager::instance().setActiveTextureUnit(TextureUnits::brdfLookup);
            derived.brdfLookup->bind();
        }
    }
    if (_textures.bumpmap) {
        StateManager::instance().setActiveTextureUnit(TextureUnits::bumpmap);
        _textures.bumpmap->bind();
    }


    if (additive) {
        StateManager::instance().withAdditiveBlending([&mesh]() { mesh->draw(); });
    } else {
        mesh->draw();
    }
}

bool ModelNodeSceneNode::isLightingEnabled() const {
    if (!isLightingEnabledByUsage(_modelSceneNode->usage())) return false;

    // Lighting is disabled for lightmapped models, unless dynamic room lighting is enabled
    if (_textures.lightmap && !isFeatureEnabled(Feature::DynamicRoomLighting)) return false;

    // Lighting is disabled for self-illuminated model nodes, e.g. sky boxes
    if (isSelfIlluminated()) return false;

    // Lighting is disabled when diffuse texture is additive
    if (_textures.diffuse && _textures.diffuse->isAdditive()) return false;

    return true;
}

void ModelNodeSceneNode::setAppliedForce(glm::vec3 force) {
    if (_modelNode->danglymesh()) {
        // Convert force from world to object space
        _danglymeshAnimation.force = _absoluteTransformInv * glm::vec4(force, 0.0f);
    }
}

glm::vec3 ModelNodeSceneNode::getOrigin() const {
    return _absoluteTransform * glm::vec4(_modelNode->getCenterOfAABB(), 1.0f);
}

void ModelNodeSceneNode::setBoneTransform(const glm::mat4 &transform) {
    _boneTransform = transform;
}

void ModelNodeSceneNode::setDiffuseTexture(const shared_ptr<Texture> &texture) {
    _textures.diffuse = texture;
    refreshMaterial();
    refreshAdditionalTextures();
}

void ModelNodeSceneNode::setAlpha(float alpha) {
    _alpha = alpha;
}

void ModelNodeSceneNode::setSelfIllumColor(glm::vec3 color) {
    _selfIllumColor = move(color);
}

} // namespace scene

} // namespace reone
