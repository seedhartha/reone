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

#include <cstdint>

#include <boost/noncopyable.hpp>

namespace reone {

namespace render {

/**
 * Abstraction over the OpenGL renderbuffer.
 */
class Renderbuffer : boost::noncopyable {
public:
    enum class PixelFormat {
        Depth,
        RGB,
        RGBA
    };

    Renderbuffer() = default;
    ~Renderbuffer();

    void init();
    void deinit();

    void bind() const;
    void unbind() const;

    /**
     * Configures this renderbuffers size and internal format. Renderbuffer must be bound.
     */
    void configure(int w, int h, PixelFormat format);

    uint32_t id() const { return _id; }

private:
    bool _inited { false };
    uint32_t _id { 0 };
};

} // namespace render

} // namespace reone
