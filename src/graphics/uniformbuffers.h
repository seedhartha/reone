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

#include "uniformbuffer.h"
#include "uniforms.h"

namespace reone {

namespace graphics {

class UniformBuffers : boost::noncopyable {
public:
    ~UniformBuffers() { deinit(); }

    void init();
    void deinit();

    void setGeneral(const std::function<void(GeneralUniforms &)> &block);
    void setText(const std::function<void(TextUniforms &)> &block);
    void setLighting(const std::function<void(LightingUniforms &)> &block);
    void setSkeletal(const std::function<void(SkeletalUniforms &)> &block);
    void setParticles(const std::function<void(ParticlesUniforms &)> &block);
    void setGrass(const std::function<void(GrassUniforms &)> &block);
    void setSSAO(const std::function<void(SSAOUniforms &)> &block);
    void setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block);

private:
    bool _inited {false};

    // Uniforms

    GeneralUniforms _general;
    TextUniforms _text;
    LightingUniforms _lighting;
    SkeletalUniforms _skeletal;
    ParticlesUniforms _particles;
    GrassUniforms _grass;
    SSAOUniforms _ssao;
    WalkmeshUniforms _walkmesh;

    // END Uniforms

    // Uniform Buffers

    std::shared_ptr<UniformBuffer> _ubGeneral;
    std::shared_ptr<UniformBuffer> _ubText;
    std::shared_ptr<UniformBuffer> _ubLighting;
    std::shared_ptr<UniformBuffer> _ubSkeletal;
    std::shared_ptr<UniformBuffer> _ubParticles;
    std::shared_ptr<UniformBuffer> _ubGrass;
    std::shared_ptr<UniformBuffer> _ubSSAO;
    std::shared_ptr<UniformBuffer> _ubWalkmesh;

    // END Uniform Buffers

    std::unique_ptr<UniformBuffer> initBuffer(const void *data, ptrdiff_t size);

    void refreshBuffer(UniformBuffer &buffer, int bindingPoint, const void *data, ptrdiff_t size);
};

} // namespace graphics

} // namespace reone
