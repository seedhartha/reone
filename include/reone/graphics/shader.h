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

#include "types.h"

namespace reone {

namespace graphics {

class Shader : boost::noncopyable {
public:
    Shader(ShaderType type, std::list<std::string> sources) :
        _type(type),
        _sources(std::move(sources)) {
    }

    ~Shader() { deinit(); }

    void init();
    void deinit();

    uint32_t nameGL() const { return _nameGL; }

private:
    ShaderType _type;
    std::list<std::string> _sources;

    bool _inited {false};

    // OpenGL

    uint32_t _nameGL {0};

    // END OpenGL
};

} // namespace graphics

} // namespace reone
