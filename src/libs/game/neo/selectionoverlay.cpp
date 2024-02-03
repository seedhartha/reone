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

#include "reone/game/neo/selectionoverlay.h"

#include "reone/game/neo/object.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/textures.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/camera.h"
#include "reone/system/checkutil.h"

using namespace reone::graphics;

namespace reone {

namespace game {

namespace neo {

void SelectionOverlay::init() {
    checkThat(!_inited, "Must not be initialized");
    _friendlyReticle = _resourceSvc.textures.get("friendlyreticle", TextureUsage::GUI);
    _friendlyReticle2 = _resourceSvc.textures.get("friendlyreticle2", TextureUsage::GUI);
    _inited = true;
}

void SelectionOverlay::deinit() {
    if (!_inited) {
        return;
    }
    _friendlyReticle.reset();
    _friendlyReticle2.reset();
    _inited = false;
}

void SelectionOverlay::render(const glm::ivec2 &screenSize) {
    if ((!_hoveredObject && !_selectedObject) || !_camera) {
        return;
    }
    if ((_hoveredObject || _selectedObject) && _camera) {
        _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpTexture));
        _graphicsSvc.uniforms.setGlobals([&screenSize](auto &globals) {
            globals.reset();
            globals.projection = glm::ortho(0.0f, static_cast<float>(screenSize.x),
                                            static_cast<float>(screenSize.y), 0.0f,
                                            0.0f, 1.0f);
        });
        _graphicsSvc.context.withBlendMode(BlendMode::Additive, [this, &screenSize]() {
            const auto &camera = *_camera->get().camera();
            if (_hoveredObject) {
                auto model = _scene.modelByExternalId(reinterpret_cast<void *>(_hoveredObject->get().id()));
                if (model) {
                    auto objectWorld = model->get().getWorldCenterOfAABB();
                    auto objectNDC = camera.projection() * camera.view() * glm::vec4 {objectWorld, 1.0f};
                    if (objectNDC.w > 0.0f) {
                        objectNDC /= objectNDC.w;
                        glm::vec3 objectScreen {screenSize.x * 0.5f * (objectNDC.x + 1.0f),
                                                screenSize.y * (1.0f - 0.5f * (objectNDC.y + 1.0f)),
                                                0.0f};
                        auto transform = glm::scale(
                            glm::translate(objectScreen - 0.5f * glm::vec3 {_friendlyReticle->width(), _friendlyReticle->height(), 0.0f}),
                            glm::vec3 {_friendlyReticle->width(), _friendlyReticle->height(), 1.0f});
                        _graphicsSvc.context.bindTexture(*_friendlyReticle);
                        _graphicsSvc.uniforms.setLocals([&transform](auto &locals) {
                            locals.reset();
                            locals.model = std::move(transform);
                        });
                        _graphicsSvc.meshRegistry.get(MeshName::quad).draw(_graphicsSvc.statistic);
                    }
                }
            }
            if (_selectedObject) {
                auto model = _scene.modelByExternalId(reinterpret_cast<void *>(_selectedObject->get().id()));
                if (model) {
                    auto objectWorld = model->get().getWorldCenterOfAABB();
                    auto objectNDC = camera.projection() * camera.view() * glm::vec4 {objectWorld, 1.0f};
                    if (objectNDC.w > 0.0f) {
                        objectNDC /= objectNDC.w;
                        glm::vec3 objectScreen {screenSize.x * 0.5f * (objectNDC.x + 1.0f),
                                                screenSize.y * (1.0f - 0.5f * (objectNDC.y + 1.0f)),
                                                0.0f};
                        auto transform = glm::scale(
                            glm::translate(objectScreen - 0.5f * glm::vec3 {_friendlyReticle2->width(), _friendlyReticle2->height(), 0.0f}),
                            glm::vec3 {_friendlyReticle2->width(), _friendlyReticle2->height(), 1.0f});
                        _graphicsSvc.context.bindTexture(*_friendlyReticle2);
                        _graphicsSvc.uniforms.setLocals([&transform](auto &locals) {
                            locals.reset();
                            locals.model = std::move(transform);
                        });
                        _graphicsSvc.meshRegistry.get(MeshName::quad).draw(_graphicsSvc.statistic);
                    }
                }
            }
        });
    }
}

} // namespace neo

} // namespace game

} // namespace reone
