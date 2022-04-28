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

struct GraphicsOptions;

class Fonts;
class GraphicsContext;
class LipAnimations;
class Meshes;
class Models;
class Pipeline;
class Shaders;
class Textures;
class Uniforms;
class Walkmeshes;
class Window;

struct GraphicsServices {
    GraphicsOptions &options;
    Fonts &fonts;
    GraphicsContext &context;
    LipAnimations &lipAnimations;
    Meshes &meshes;
    Models &models;
    Pipeline &pipeline;
    Shaders &shaders;
    Textures &textures;
    Uniforms &uniforms;
    Walkmeshes &walkmeshes;
    Window &window;

    GraphicsServices(
        GraphicsOptions &options,
        Fonts &fonts,
        GraphicsContext &graphicsContext,
        LipAnimations &lipAnimations,
        Meshes &meshes,
        Models &models,
        Pipeline &pipeline,
        Shaders &shaders,
        Textures &textures,
        Uniforms &uniforms,
        Walkmeshes &walkmeshes,
        Window &window) :
        options(options),
        fonts(fonts),
        context(graphicsContext),
        lipAnimations(lipAnimations),
        meshes(meshes),
        models(models),
        pipeline(pipeline),
        shaders(shaders),
        textures(textures),
        uniforms(uniforms),
        walkmeshes(walkmeshes),
        window(window) {
    }
};

} // namespace graphics

} // namespace reone
