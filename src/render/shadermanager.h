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
    GUIText
};

struct ShaderUniforms {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 cameraPosition;
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

} // namespace render

} // namespace reone
