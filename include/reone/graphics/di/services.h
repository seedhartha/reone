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

#include "../context.h"
#include "../fonts.h"
#include "../lipanimations.h"
#include "../meshes.h"
#include "../models.h"
#include "../pipeline.h"
#include "../shaders.h"
#include "../textures.h"
#include "../uniforms.h"
#include "../walkmeshes.h"
#include "../window.h"

namespace reone {

namespace graphics {

struct GraphicsServices {
    IFonts &fonts;
    IGraphicsContext &context;
    ILipAnimations &lipAnimations;
    IMeshes &meshes;
    IModels &models;
    IPipeline &pipeline;
    IShaders &shaders;
    ITextures &textures;
    IUniforms &uniforms;
    IWalkmeshes &walkmeshes;
    IWindow &window;

    GraphicsServices(
        IFonts &fonts,
        IGraphicsContext &graphicsContext,
        ILipAnimations &lipAnimations,
        IMeshes &meshes,
        IModels &models,
        IPipeline &pipeline,
        IShaders &shaders,
        ITextures &textures,
        IUniforms &uniforms,
        IWalkmeshes &walkmeshes,
        IWindow &window) :
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
