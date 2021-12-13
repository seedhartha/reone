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

namespace reone {

namespace graphics {

class UniformBuffers : boost::noncopyable {
public:
    ~UniformBuffers() { deinit(); }

    void init();
    void deinit();

    UniformBuffer &combined() const { return *_combined; }
    UniformBuffer &text() const { return *_text; }
    UniformBuffer &lighting() const { return *_lighting; }
    UniformBuffer &skeletal() const { return *_skeletal; }
    UniformBuffer &particles() const { return *_particles; }
    UniformBuffer &grass() const { return *_grass; }
    UniformBuffer &danglymesh() const { return *_danglymesh; }

    std::shared_ptr<UniformBuffer> combinedPtr() const { return _combined; }
    std::shared_ptr<UniformBuffer> textPtr() const { return _text; }
    std::shared_ptr<UniformBuffer> lightingPtr() const { return _lighting; }
    std::shared_ptr<UniformBuffer> skeletalPtr() const { return _skeletal; }
    std::shared_ptr<UniformBuffer> particlesPtr() const { return _particles; }
    std::shared_ptr<UniformBuffer> grassPtr() const { return _grass; }
    std::shared_ptr<UniformBuffer> danglymeshPtr() const { return _danglymesh; }

private:
    bool _inited {false};

    std::shared_ptr<UniformBuffer> _combined;
    std::shared_ptr<UniformBuffer> _text;
    std::shared_ptr<UniformBuffer> _lighting;
    std::shared_ptr<UniformBuffer> _skeletal;
    std::shared_ptr<UniformBuffer> _particles;
    std::shared_ptr<UniformBuffer> _grass;
    std::shared_ptr<UniformBuffer> _danglymesh;

    std::unique_ptr<UniformBuffer> initBuffer(const void *data, ptrdiff_t size);
};

} // namespace graphics

} // namespace reone
