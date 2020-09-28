/*
 * Copyright © 2020 Vsevolod Kremianskii
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

namespace reone {

namespace render {

class Framebuffer {
public:
    Framebuffer(int w, int h);
    ~Framebuffer();

    void init();
    void deinit();
    void bind();
    void unbind();
    void bindTexture();
    void unbindTexture();

    int width() const;
    int height() const;

private:
    int _width { 0 };
    int _height { 0 };
    bool _inited { false };
    uint32_t _framebuffer { 0 };
    uint32_t _texture { 0 };
    uint32_t _renderbuffer { 0 };
};

} // namespace render

} // namespace reone
