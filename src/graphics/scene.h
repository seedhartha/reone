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

#pragma once

namespace reone {

namespace graphics {

struct Uniforms;

class IScene {
public:
    virtual ~IScene() = default;

    virtual void draw() = 0;
    virtual void drawShadows() = 0;

    virtual const glm::vec3 &ambientLightColor() const = 0;

    // Camera

    virtual bool hasCamera() const = 0;

    virtual glm::vec3 cameraPosition() const = 0;
    virtual float cameraFieldOfView() const = 0;
    virtual float cameraNearPlane() const = 0;
    virtual float cameraFarPlane() const = 0;
    virtual const glm::mat4 &cameraProjection() const = 0;
    virtual const glm::mat4 &cameraView() const = 0;

    // END Camera

    // Fog

    virtual float fogNear() const = 0;
    virtual float fogFar() const = 0;
    virtual const glm::vec3 &fogColor() const = 0;

    // END Fog

    // Shadows

    virtual bool hasShadowLight() const = 0;
    virtual bool isShadowLightDirectional() const = 0;

    virtual glm::vec3 shadowLightPosition() const = 0;
    virtual float shadowFadeFactor() const = 0;

    // END Shadows
};

} // namespace graphics

} // namespace reone
