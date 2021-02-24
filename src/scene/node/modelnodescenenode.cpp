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
#include "../../render/featureutil.h"
#include "../../render/materials.h"
#include "../../render/pbribl.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"
#include "../../render/textures.h"

#include "../scenegraph.h"

#include "cameranode.h"
#include "lightnode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr float kUvAnimationSpeed = 250.0f;

ModelNodeSceneNode::ModelNodeSceneNode(SceneGraph *sceneGraph, const ModelSceneNode *modelSceneNode, ModelNode *modelNode) :
    SceneNode(sceneGraph),
    _modelSceneNode(modelSceneNode),
    _modelNode(modelNode) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
    if (!modelNode) {
        throw invalid_argument("modelNode must not be null");
    }

    _alpha = _modelNode->alpha();
    _selfIllumColor = _modelNode->selfIllumColor();

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
                _bumpmapFrame = glm::round((frameCount - 1) * (_bumpmapTime / length));
                if (_bumpmapTime == length) {
                    _bumpmapTime = 0.0f;
                }
            }
        }
    }
    SceneNode::update(dt);
}

bool ModelNodeSceneNode::shouldRender() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    return mesh && mesh->shouldRender();
}

bool ModelNodeSceneNode::shouldCastShadows() const {
    if (_modelSceneNode->classification() == ModelSceneNode::Classification::Door) return false;

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

static bool isLightingEnabledByClassification(ModelSceneNode::Classification classification) {
    if (classification == ModelSceneNode::Classification::Room) {
        return isFeatureEnabled(Feature::DynamicRoomLighting);
    }
    return classification != ModelSceneNode::Classification::Projectile;
}

bool ModelNodeSceneNode::isSelfIlluminated() const {
    return glm::dot(_selfIllumColor, _selfIllumColor) > 0.0f;
}

static bool isReceivingShadows(const ModelSceneNode &model, const ModelNodeSceneNode &modelNode) {
    // Only room models receive shadows, unless model node is self-illuminated
    return
        model.classification() == ModelSceneNode::Classification::Room &&
        !modelNode.isSelfIlluminated();
}

void ModelNodeSceneNode::renderSingle(bool shadowPass) {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    // Setup shaders

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    if (isFeatureEnabled(Feature::HDR)) {
        uniforms.general.featureMask |= UniformFeatureFlags::hdr;
    }
    uniforms.general.model = _absoluteTransform;
    uniforms.general.alpha = _modelSceneNode->alpha() * _alpha;
    uniforms.general.ambientColor = glm::vec4(_sceneGraph->ambientLightColor(), 1.0f);
    uniforms.general.exposure = _sceneGraph->exposure();

    ShaderProgram program;

    if (shadowPass) {
        program = ShaderProgram::SimpleDepth;

    } else {
        program = isFeatureEnabled(Feature::PBR) ? ShaderProgram::ModelPBR : ShaderProgram::ModelBlinnPhong;

        if (_textures.diffuse) {
            uniforms.general.featureMask |= UniformFeatureFlags::diffuse;
        }

        if (_textures.envmap) {
            uniforms.general.featureMask |= UniformFeatureFlags::envmap;

            if (isFeatureEnabled(Feature::PBR)) {
                bool derived = PBRIBL::instance().contains(_textures.envmap.get());
                if (derived) {
                    uniforms.general.featureMask |= UniformFeatureFlags::pbrIbl;
                }
            }
        }

        if (_textures.lightmap) {
            uniforms.general.featureMask |= UniformFeatureFlags::lightmap;
        }

        if (_textures.bumpmap) {
            uniforms.general.featureMask |= UniformFeatureFlags::bumpmap;
            uniforms.bumpmap.grayscale = _textures.bumpmap->isGrayscale();
            uniforms.bumpmap.scaling = _textures.bumpmap->features().bumpMapScaling;
            uniforms.bumpmap.gridSize = glm::vec2(_textures.bumpmap->features().numX, _textures.bumpmap->features().numY);
            uniforms.bumpmap.frame = _bumpmapFrame;
            uniforms.bumpmap.swizzled = mesh->isBumpmapSwizzled();
        }

        bool receivesShadows = isReceivingShadows(*_modelSceneNode, *this);
        if (receivesShadows) {
            uniforms.general.featureMask |= UniformFeatureFlags::shadows;
        }

        shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
        if (skin) {
            uniforms.general.featureMask |= UniformFeatureFlags::skeletal;

            for (int i = 0; i < kMaxBoneCount; ++i) {
                uniforms.skeletal.bones[i] = glm::mat4(1.0f);
            }
            for (auto &pair : skin->nodeIdxByBoneIdx) {
                uint16_t boneIdx = pair.first;
                uint16_t nodeIdx = pair.second;

                ModelNodeSceneNode *bone = _modelSceneNode->getModelNodeByIndex(nodeIdx);
                if (bone) {
                    uniforms.skeletal.bones[boneIdx] = _modelNode->absoluteTransformInverse() * bone->boneTransform() * _modelNode->absoluteTransform();
                }
            }
        }

        if (isSelfIlluminated()) {
            uniforms.general.featureMask |= UniformFeatureFlags::selfIllum;
            uniforms.general.selfIllumColor = glm::vec4(_selfIllumColor, 1.0f);
        }
        if (isLightingEnabled()) {
            const vector<LightSceneNode *> &lights = _modelSceneNode->lightsAffectedBy();

            uniforms.general.featureMask |= UniformFeatureFlags::lighting;
            if (_material.custom) {
                uniforms.general.featureMask |= UniformFeatureFlags::customMat;
            }
            uniforms.lighting.materialAmbient = glm::vec4(mesh->ambientColor(), 1.0f);
            uniforms.lighting.materialDiffuse = glm::vec4(mesh->diffuseColor(), 1.0f);
            uniforms.lighting.materialSpecular = _material.specular;
            uniforms.lighting.materialShininess = _material.shininess;
            uniforms.lighting.materialMetallic = _material.metallic;
            uniforms.lighting.materialRoughness = _material.roughness;
            uniforms.lighting.lightCount = static_cast<int>(lights.size());

            for (int i = 0; i < uniforms.lighting.lightCount; ++i) {
                ShaderLight &shaderLight = uniforms.lighting.lights[i];
                shaderLight.position = lights[i]->absoluteTransform()[3];
                shaderLight.color = glm::vec4(lights[i]->color(), 1.0f);
                shaderLight.multiplier = lights[i]->multiplier();
                shaderLight.radius = lights[i]->radius();
            }
        }

        if (_textures.diffuse) {
            uniforms.general.uvOffset = _uvOffset;

            float waterAlpha = _textures.diffuse->features().waterAlpha;
            if (waterAlpha != -1.0f) {
                uniforms.general.featureMask |= UniformFeatureFlags::water;
                uniforms.general.waterAlpha = waterAlpha;
            }
        }
    }

    Shaders::instance().activate(program, uniforms);



    bool additive = false;

    // Setup textures

    if (_textures.diffuse) {
        setActiveTextureUnit(TextureUnits::diffuse);
        _textures.diffuse->bind();
        additive = _textures.diffuse->isAdditive();
    }
    if (_textures.lightmap) {
        setActiveTextureUnit(TextureUnits::lightmap);
        _textures.lightmap->bind();
    }
    if (_textures.envmap) {
        setActiveTextureUnit(TextureUnits::envmap);
        _textures.envmap->bind();

        PBRIBL::Derived derived;
        if (PBRIBL::instance().getDerived(_textures.envmap.get(), derived)) {
            setActiveTextureUnit(TextureUnits::irradianceMap);
            derived.irradianceMap->bind();
            setActiveTextureUnit(TextureUnits::prefilterMap);
            derived.prefilterMap->bind();
            setActiveTextureUnit(TextureUnits::brdfLookup);
            derived.brdfLookup->bind();
        }
    }
    if (_textures.bumpmap) {
        setActiveTextureUnit(TextureUnits::bumpmap);
        _textures.bumpmap->bind();
    }


    if (additive) {
        withAdditiveBlending([&mesh]() { mesh->render(); });
    } else {
        mesh->render();
    }
}

bool ModelNodeSceneNode::isLightingEnabled() const {
    if (!isLightingEnabledByClassification(_modelSceneNode->classification())) return false;

    // Lighting is disabled for lightmapped models, unless dynamic room lighting is enabled
    if (_textures.lightmap && !isFeatureEnabled(Feature::DynamicRoomLighting)) return false;

    // Lighting is disabled for self-illuminated model nodes, e.g. sky boxes
    if (isSelfIlluminated()) return false;

    // Lighting is disabled when diffuse texture is additive
    if (_textures.diffuse && _textures.diffuse->isAdditive()) return false;

    return true;
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
