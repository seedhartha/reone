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

#include "uniformbuffers.h"

#include "shaders.h"

using namespace std;

namespace reone {

namespace graphics {

void UniformBuffers::init() {
    static CombinedUniforms defaultsCombined;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    static DanglymeshUniforms defaultsDanglymesh;

    _combined = initBuffer(&defaultsCombined, sizeof(CombinedUniforms));
    _text = initBuffer(&defaultsText, sizeof(TextUniforms));
    _lighting = initBuffer(&defaultsLighting, sizeof(LightingUniforms));
    _skeletal = initBuffer(&defaultsSkeletal, sizeof(SkeletalUniforms));
    _particles = initBuffer(&defaultsParticles, sizeof(ParticlesUniforms));
    _grass = initBuffer(&defaultsGrass, sizeof(GrassUniforms));
    _danglymesh = initBuffer(&defaultsDanglymesh, sizeof(DanglymeshUniforms));

    _inited = true;
}

void UniformBuffers::deinit() {
    if (!_inited) {
        return;
    }
    _combined.reset();
    _text.reset();
    _lighting.reset();
    _skeletal.reset();
    _particles.reset();
    _grass.reset();
    _danglymesh.reset();
    _inited = false;
}

unique_ptr<UniformBuffer> UniformBuffers::initBuffer(const void *data, ptrdiff_t size) {
    auto buf = make_unique<UniformBuffer>();
    buf->setData(data, size);
    buf->init();
    return move(buf);
}

} // namespace graphics

} // namespace reone
