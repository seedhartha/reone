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

#include "shader.h"

namespace reone {

namespace graphics {

class ShaderProgram : boost::noncopyable {
public:
    ShaderProgram(std::vector<std::shared_ptr<Shader>> shaders) :
        _shaders(std::move(shaders)) {
    }

    ~ShaderProgram() { deinit(); }

    void init();
    void deinit();

    void use();

    void bindUniformBlock(const std::string &name, int bindingPoint);

    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, const glm::vec2 &v);
    void setUniform(const std::string &name, const glm::vec3 &v);
    void setUniform(const std::string &name, const glm::vec4 &v);
    void setUniform(const std::string &name, const glm::mat4 &m);
    void setUniform(const std::string &name, const std::vector<glm::mat4> &arr);
    void setUniform(const std::string &name, const std::function<void(int)> &setter);

private:
    std::vector<std::shared_ptr<Shader>> _shaders;

    bool _inited {false};

    // OpenGL

    uint32_t _nameGL {0};
    std::map<std::string, int> _uniformLocations;

    // END OpenGL
};

} // namespace graphics

} // namespace reone
