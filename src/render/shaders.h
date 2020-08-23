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

#include <map>
#include <string>
#include <vector>

#include "glm/glm.hpp"

namespace reone {

namespace render {

enum class ShaderProgram {
    None,
    BasicWhite,
    BasicDiffuse,
    BasicDiffuseEnvmap,
    BasicDiffuseBumpyShiny,
    BasicDiffuseLightmap,
    BasicDiffuseLightmapEnvmap,
    BasicDiffuseLightmapBumpyShiny,
    SkeletalDiffuse,
    SkeletalDiffuseEnvmap,
    SkeletalDiffuseBumpyShiny,
    SkeletalDiffuseBumpmap,
    GUIText
};

struct ShaderUniforms {
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
    glm::vec3 cameraPosition { 0.0f };
};

class ShaderManager {
public:
    static ShaderManager &instance();

    void initGL();
    void deinitGL();
    void activate(ShaderProgram program);
    void deactivate();
    void setGlobalUniforms(const ShaderUniforms &uniforms);
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, const glm::vec3 &v);
    void setUniform(const std::string &name, const glm::mat4 &m);
    void setUniform(const std::string &name, const std::vector<glm::mat4> &arr);

private:
    enum class ShaderName {
        VertexBasic,
        VertexSkeletal,
        VertexGUI,
        FragmentWhite,
        FragmentDiffuse,
        FragmentDiffuseEnvmap,
        FragmentDiffuseBumpyShiny,
        FragmentDiffuseLightmap,
        FragmentDiffuseLightmapEnvmap,
        FragmentDiffuseLightmapBumpyShiny,
        FragmentDiffuseBumpmap,
        FragmentText
    };

    std::map<ShaderName, unsigned int> _shaders;
    std::map<ShaderProgram, unsigned int> _programs;
    ShaderProgram _activeProgram { ShaderProgram::None };
    unsigned int _activeOrdinal { 0 };

    ShaderManager() = default;
    ShaderManager(const ShaderManager &) = delete;
    ~ShaderManager();

    ShaderManager &operator=(const ShaderManager &) = delete;

    void initShader(ShaderName name, unsigned int type, const char *source);
    void initProgram(ShaderProgram program, ShaderName vertexShader, ShaderName fragmentShader);
    unsigned int getOrdinal(ShaderProgram program) const;
    void setUniform(unsigned int ordinal, const std::string &name, int value);
    void setUniform(unsigned int ordinal, const std::string &name, float value);
    void setUniform(unsigned int ordinal, const std::string &name, const glm::vec3 &v);
    void setUniform(unsigned int ordinal, const std::string &name, const glm::mat4 &m);
    void setUniform(unsigned int ordinal, const std::string &name, const std::vector<glm::mat4> &arr);
};

#define ShaderMan render::ShaderManager::instance()

} // namespace render

} // namespace reone
