/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "context.h"
#include "fonts.h"
#include "lipanimations.h"
#include "meshes.h"
#include "models.h"
#include "pipeline.h"
#include "shaders.h"
#include "textures.h"
#include "uniforms.h"
#include "walkmeshes.h"
#include "window.h"

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

    Fonts &defaultFonts() {
        auto casted = dynamic_cast<Fonts *>(&fonts);
        if (!casted) {
            throw std::logic_error("Illegal Fonts implementation");
        }
        return *casted;
    }

    GraphicsContext &defaultContext() {
        auto casted = dynamic_cast<GraphicsContext *>(&context);
        if (!casted) {
            throw std::logic_error("Illegal GraphicsContext implementation");
        }
        return *casted;
    }

    LipAnimations &defaultLipAnimations() {
        auto casted = dynamic_cast<LipAnimations *>(&lipAnimations);
        if (!casted) {
            throw std::logic_error("Illegal LipAnimations implementation");
        }
        return *casted;
    }

    Meshes &defaultMeshes() {
        auto casted = dynamic_cast<Meshes *>(&meshes);
        if (!casted) {
            throw std::logic_error("Illegal Meshes implementation");
        }
        return *casted;
    }

    Models &defaultModels() {
        auto casted = dynamic_cast<Models *>(&models);
        if (!casted) {
            throw std::logic_error("Illegal Models implementation");
        }
        return *casted;
    }

    Pipeline &defaultPipeline() {
        auto casted = dynamic_cast<Pipeline *>(&pipeline);
        if (!casted) {
            throw std::logic_error("Illegal Pipeline implementation");
        }
        return *casted;
    }

    Shaders &defaultShaders() {
        auto casted = dynamic_cast<Shaders *>(&shaders);
        if (!casted) {
            throw std::logic_error("Illegal Shaders implementation");
        }
        return *casted;
    }

    Textures &defaultTextures() {
        auto casted = dynamic_cast<Textures *>(&textures);
        if (!casted) {
            throw std::logic_error("Illegal Textures implementation");
        }
        return *casted;
    }

    Uniforms &defaultUniforms() {
        auto casted = dynamic_cast<Uniforms *>(&uniforms);
        if (!casted) {
            throw std::logic_error("Illegal Uniforms implementation");
        }
        return *casted;
    }

    Walkmeshes &defaultWalkmeshes() {
        auto casted = dynamic_cast<Walkmeshes *>(&walkmeshes);
        if (!casted) {
            throw std::logic_error("Illegal Walkmeshes implementation");
        }
        return *casted;
    }

    Window &defaultWindow() {
        auto casted = dynamic_cast<Window *>(&window);
        if (!casted) {
            throw std::logic_error("Illegal Window implementation");
        }
        return *casted;
    }
};

} // namespace graphics

} // namespace reone
