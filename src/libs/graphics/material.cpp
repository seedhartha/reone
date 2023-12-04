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

#include "reone/graphics/material.h"

#include "reone/graphics/context.h"
#include "reone/graphics/types.h"
#include "reone/graphics/uniformbuffer.h"
#include "reone/graphics/uniforms.h"

namespace reone {

namespace graphics {

void Material::apply(IGraphicsContext &context) {
    context.useProgram(_program);
    for (auto &[unit, texture] : _textures) {
        context.bind(*texture, unit);
    }
}

void ModelMaterial::apply(IGraphicsContext &context) {
    Material::apply(context);
    auto &uboLocals = context.uniformBufferAt(UniformBlockBindingPoints::locals);
    uboLocals.bind(UniformBlockBindingPoints::locals);
    uboLocals.setData(&_localsUniforms, sizeof(LocalsUniforms));
}

void SkinnedModelMaterial::apply(IGraphicsContext &context) {
    ModelMaterial::apply(context);
    auto &uboSkeletal = context.uniformBufferAt(UniformBlockBindingPoints::skeletal);
    uboSkeletal.bind(UniformBlockBindingPoints::skeletal);
    uboSkeletal.setData(&_skeletalUniforms, sizeof(SkeletalUniforms));
}

} // namespace graphics

} // namespace reone
