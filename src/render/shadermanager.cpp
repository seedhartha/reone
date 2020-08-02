#include "shadermanager.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

namespace reone {

namespace render {

static const GLchar kBasicVertexShader[] = "#version 330\n\
        \n\
        uniform mat4 projection;\n\
        uniform mat4 view;\n\
        uniform mat4 model;\n\
        \n\
        layout(location = 0) in vec3 position;\n\
        layout(location = 1) in vec3 normal;\n\
        layout(location = 2) in vec2 texCoords;\n\
        layout(location = 3) in vec2 lightmapCoords;\n\
        \n\
        out vec3 fragPosition;\n\
        out vec3 fragNormal;\n\
        out vec2 fragTexCoords;\n\
        out vec2 fragLightmapCoords;\n\
        \n\
        void main() {\n\
            gl_Position = projection * view * model * vec4(position, 1);\n\
            fragPosition = vec3(model * vec4(position, 1));\n\
            fragNormal = mat3(transpose(inverse(model))) * normal;\n\
            fragTexCoords = texCoords;\n\
            fragLightmapCoords = lightmapCoords;\n\
        }";

static const GLchar kSkeletalVertexShader[] = "#version 330\n\
        \n\
        const int MAX_BONES = 128;\n\
        \n\
        uniform mat4 projection;\n\
        uniform mat4 view;\n\
        uniform mat4 model;\n\
        uniform mat4 absTransform;\n\
        uniform mat4 absTransformInv;\n\
        uniform mat4 bones[MAX_BONES];\n\
        \n\
        layout(location = 0) in vec3 position;\n\
        layout(location = 1) in vec3 normal;\n\
        layout(location = 2) in vec2 texCoords;\n\
        layout(location = 4) in vec4 boneWeights;\n\
        layout(location = 5) in vec4 boneIndices;\n\
        \n\
        out vec3 fragPosition;\n\
        out vec3 fragNormal;\n\
        out vec2 fragTexCoords;\n\
        \n\
        void main() {\n\
            float weight0 = boneWeights.x;\n\
            float weight1 = boneWeights.y;\n\
            float weight2 = boneWeights.z;\n\
            float weight3 = boneWeights.w;\n\
            \n\
            int index0 = int(boneIndices.x);\n\
            int index1 = int(boneIndices.y);\n\
            int index2 = int(boneIndices.z);\n\
            int index3 = int(boneIndices.w);\n\
            \n\
            vec3 newPosition = vec3(0, 0, 0);\n\
            vec4 position4 = vec4(position, 1);\n\
            \n\
            if (index0 != -1) {\n\
                newPosition += weight0 * (absTransformInv * bones[index0] * absTransform * position4).xyz;\n\
            }\n\
            if (index1 != -1) {\n\
                newPosition += weight1 * (absTransformInv * bones[index1] * absTransform * position4).xyz;\n\
            }\n\
            if (index2 != -1) {\n\
                newPosition += weight2 * (absTransformInv * bones[index2] * absTransform * position4).xyz;\n\
            }\n\
            if (index3 != -1) {\n\
                newPosition += weight3 * (absTransformInv * bones[index3] * absTransform * position4).xyz;\n\
            }\n\
            \n\
            gl_Position = projection * view * model * vec4(newPosition, 1);\n\
            fragPosition = vec3(model * vec4(position, 1));\n\
            fragNormal = mat3(transpose(inverse(model))) * normal;\n\
            fragTexCoords = texCoords;\n\
        }";

static const GLchar kGUIVertexShader[] = "#version 330\n\
        \n\
        uniform mat4 projection;\n\
        uniform mat4 view;\n\
        uniform mat4 model;\n\
        \n\
        layout(location = 0) in vec3 position;\n\
        layout(location = 2) in vec2 texCoords;\n\
        \n\
        out vec2 fragTexCoords;\n\
        \n\
        void main() {\n\
            gl_Position = projection * view * model * vec4(position, 1);\n\
            fragTexCoords = texCoords;\n\
        }";

static const GLchar kWhiteFragmentShader[] = "#version 330\n\
        \n\
        uniform float alpha;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            color = vec4(1, 1, 1, alpha);\n\
        }";

static const GLchar kDiffuseFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D diffuse;\n\
        uniform vec3 color;\n\
        uniform float alpha;\n\
        \n\
        in vec2 fragTexCoords;\n\
        \n\
        out vec4 fragColor;\n\
        \n\
        void main() {\n\
            vec4 objectColor = texture(diffuse, fragTexCoords);\n\
            fragColor = vec4(objectColor.rgb * color, alpha * objectColor.a);\n\
        }";

static const GLchar kDiffuseEnvmapFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D diffuse;\n\
        uniform samplerCube envmap;\n\
        uniform vec3 cameraPosition;\n\
        uniform float alpha;\n\
        \n\
        in vec3 fragPosition;\n\
        in vec3 fragNormal;\n\
        in vec2 fragTexCoords;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            vec3 I = normalize(fragPosition - cameraPosition);\n\
            vec3 R = reflect(I, normalize(fragNormal));\n\
            vec4 envmapSample = texture(envmap, R);\n\
            \n\
            vec4 diffuseSample = texture(diffuse, fragTexCoords);\n\
            vec4 objectColor = diffuseSample;\n\
            objectColor += envmapSample * (1 - diffuseSample.a);\n\
            \n\
            color = vec4(objectColor.rgb, alpha * objectColor.a);\n\
        }";

static const GLchar kDiffuseBumpyShinyFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D diffuse;\n\
        uniform samplerCube bumpyShiny;\n\
        uniform vec3 cameraPosition;\n\
        uniform float alpha;\n\
        \n\
        in vec3 fragPosition;\n\
        in vec3 fragNormal;\n\
        in vec2 fragTexCoords;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            vec3 I = normalize(fragPosition - cameraPosition);\n\
            vec3 R = reflect(I, fragNormal);\n\
            vec4 bumpyShinySample = texture(bumpyShiny, R);\n\
            \n\
            vec4 diffuseSample = texture(diffuse, fragTexCoords);\n\
            vec4 objectColor = vec4(diffuseSample.rgb, 1);\n\
            objectColor += bumpyShinySample * (1 - diffuseSample.a);\n\
            \n\
            color = vec4(objectColor.rgb, alpha * objectColor.a);\n\
        }";

static const GLchar kDiffuseLightmapFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D diffuse;\n\
        uniform sampler2D lightmap;\n\
        uniform float alpha;\n\
        \n\
        in vec2 fragTexCoords;\n\
        in vec2 fragLightmapCoords;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            vec4 diffuseSample = texture(diffuse, fragTexCoords);\n\
            vec4 lightmapSample = texture(lightmap, fragLightmapCoords);\n\
            \n\
            vec4 objectColor = vec4((diffuseSample.rgb * lightmapSample.rgb), diffuseSample.a);\n\
            color = vec4(objectColor.rgb, alpha * objectColor.a);\n\
        }";

static const GLchar kDiffuseLightmapEnvmapFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D diffuse;\n\
        uniform sampler2D lightmap;\n\
        uniform samplerCube envmap;\n\
        uniform vec3 cameraPosition;\n\
        uniform float alpha;\n\
        \n\
        in vec3 fragPosition;\n\
        in vec3 fragNormal;\n\
        in vec2 fragTexCoords;\n\
        in vec2 fragLightmapCoords;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            vec3 I = normalize(fragPosition - cameraPosition);\n\
            vec3 R = reflect(I, normalize(fragNormal));\n\
            vec4 envmapSample = texture(envmap, R);\n\
            \n\
            vec4 diffuseSample = texture(diffuse, fragTexCoords);\n\
            vec4 lightmapSample = texture(lightmap, fragLightmapCoords);\n\
            \n\
            vec4 objectColor = diffuseSample * lightmapSample;\n\
            objectColor += envmapSample * (1 - diffuseSample.a);\n\
            \n\
            color = vec4(objectColor.rgb, alpha * objectColor.a);\n\
        }";

static const GLchar kDiffuseLightmapBumpyShinyFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D diffuse;\n\
        uniform sampler2D lightmap;\n\
        uniform samplerCube bumpyShiny;\n\
        uniform vec3 cameraPosition;\n\
        uniform float alpha;\n\
        \n\
        in vec3 fragPosition;\n\
        in vec3 fragNormal;\n\
        in vec2 fragTexCoords;\n\
        in vec2 fragLightmapCoords;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            vec3 I = normalize(fragPosition - cameraPosition);\n\
            vec3 R = reflect(I, fragNormal);\n\
            vec4 bumpyShinySample = texture(bumpyShiny, R);\n\
            \n\
            vec4 diffuseSample = texture(diffuse, fragTexCoords);\n\
            vec4 lightmapSample = texture(lightmap, fragLightmapCoords);\n\
            \n\
            vec4 objectColor = diffuseSample * lightmapSample;\n\
            objectColor += bumpyShinySample * (1 - diffuseSample.a);\n\
            color = vec4(objectColor.rgb, alpha * objectColor.a);\n\
        }";

static const GLchar kTextFragmentShader[] = "#version 330\n\
        \n\
        uniform sampler2D font;\n\
        uniform vec3 textColor;\n\
        \n\
        in vec2 fragTexCoords;\n\
        \n\
        out vec4 color;\n\
        \n\
        void main() {\n\
            color = vec4(textColor, texture(font, fragTexCoords).a);\n\
        }";

ShaderManager &ShaderManager::instance() {
    static ShaderManager instance;
    return instance;
}

void ShaderManager::initGL() {
    initShader(ShaderName::VertexBasic, GL_VERTEX_SHADER, kBasicVertexShader);
    initShader(ShaderName::VertexSkeletal, GL_VERTEX_SHADER, kSkeletalVertexShader);
    initShader(ShaderName::VertexGUI, GL_VERTEX_SHADER, kGUIVertexShader);
    initShader(ShaderName::FragmentWhite, GL_FRAGMENT_SHADER, kWhiteFragmentShader);
    initShader(ShaderName::FragmentDiffuse, GL_FRAGMENT_SHADER, kDiffuseFragmentShader);
    initShader(ShaderName::FragmentDiffuseEnvmap, GL_FRAGMENT_SHADER, kDiffuseEnvmapFragmentShader);
    initShader(ShaderName::FragmentDiffuseBumpyShiny, GL_FRAGMENT_SHADER, kDiffuseBumpyShinyFragmentShader);
    initShader(ShaderName::FragmentDiffuseLightmap, GL_FRAGMENT_SHADER, kDiffuseLightmapFragmentShader);
    initShader(ShaderName::FragmentDiffuseLightmapEnvmap, GL_FRAGMENT_SHADER, kDiffuseLightmapEnvmapFragmentShader);
    initShader(ShaderName::FragmentDiffuseLightmapBumpyShiny, GL_FRAGMENT_SHADER, kDiffuseLightmapBumpyShinyFragmentShader);
    initShader(ShaderName::FragmentText, GL_FRAGMENT_SHADER, kTextFragmentShader);

    initProgram(ShaderProgram::BasicWhite, ShaderName::VertexBasic, ShaderName::FragmentWhite);
    initProgram(ShaderProgram::BasicDiffuse, ShaderName::VertexBasic, ShaderName::FragmentDiffuse);
    initProgram(ShaderProgram::BasicDiffuseEnvmap, ShaderName::VertexBasic, ShaderName::FragmentDiffuseEnvmap);
    initProgram(ShaderProgram::BasicDiffuseBumpyShiny, ShaderName::VertexBasic, ShaderName::FragmentDiffuseBumpyShiny);
    initProgram(ShaderProgram::BasicDiffuseLightmap, ShaderName::VertexBasic, ShaderName::FragmentDiffuseLightmap);
    initProgram(ShaderProgram::BasicDiffuseLightmapEnvmap, ShaderName::VertexBasic, ShaderName::FragmentDiffuseLightmapEnvmap);
    initProgram(ShaderProgram::BasicDiffuseLightmapBumpyShiny, ShaderName::VertexBasic, ShaderName::FragmentDiffuseLightmapBumpyShiny);
    initProgram(ShaderProgram::SkeletalDiffuse, ShaderName::VertexSkeletal, ShaderName::FragmentDiffuse);
    initProgram(ShaderProgram::SkeletalDiffuseEnvmap, ShaderName::VertexSkeletal, ShaderName::FragmentDiffuseEnvmap);
    initProgram(ShaderProgram::SkeletalDiffuseBumpyShiny, ShaderName::VertexSkeletal, ShaderName::FragmentDiffuseBumpyShiny);
    initProgram(ShaderProgram::GUIText, ShaderName::VertexGUI, ShaderName::FragmentText);
}

void ShaderManager::initShader(ShaderName name, unsigned int type, const char *source) {
    GLuint shader = glCreateShader(type);
    GLint success;
    char log[512];
    GLsizei logSize;

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(log), &logSize, log);
        throw std::runtime_error("Shader compilation failed: " + std::string(log, logSize));
    }

    _shaders.insert(std::make_pair(name, shader));
}

void ShaderManager::initProgram(ShaderProgram program, ShaderName vertexShader, ShaderName fragmentShader) {
    unsigned int vsOrdinal = _shaders.find(vertexShader)->second;
    unsigned int fsOrdinal = _shaders.find(fragmentShader)->second;

    GLuint ordinal = glCreateProgram();
    GLint success;
    char log[512];
    GLsizei logSize;

    glAttachShader(ordinal, vsOrdinal);
    glAttachShader(ordinal, fsOrdinal);
    glLinkProgram(ordinal);
    glGetProgramiv(ordinal, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ordinal, sizeof(log), &logSize, log);
        throw std::runtime_error("Program linking failed: " + std::string(log, logSize));
    }

    _programs.insert(std::make_pair(program, ordinal));
}

ShaderManager::~ShaderManager() {
    deinitGL();
}

void ShaderManager::deinitGL() {
    for (auto &pair :_programs) {
        glDeleteProgram(pair.second);
    }
    _programs.clear();

    for (auto &pair : _shaders) {
        glDeleteShader(pair.second);
    }
    _shaders.clear();
}

void ShaderManager::activate(ShaderProgram program) {
    if (_activeProgram == program) return;

    unsigned int ordinal = getOrdinal(program);
    glUseProgram(ordinal);
    _activeProgram = program;
    _activeOrdinal = ordinal;
}

unsigned int ShaderManager::getOrdinal(ShaderProgram program) const {
    auto it = _programs.find(program);
    if (it == _programs.end()) {
        throw std::invalid_argument("Shader program not found: " + std::to_string(static_cast<int>(program)));
    }
    return it->second;
}

void ShaderManager::deactivate() {
    if (_activeProgram != ShaderProgram::None) {
        glUseProgram(0);
        _activeProgram = ShaderProgram::None;
        _activeOrdinal = 0;
    }
}

void ShaderManager::setGlobalUniforms(const ShaderUniforms &uniforms) {
    for (auto &pair : _programs) {
        unsigned int ordinal = pair.second;
        glUseProgram(ordinal);
        setUniform(ordinal, "projection", uniforms.projection);
        setUniform(ordinal, "view", uniforms.view);
        setUniform(ordinal, "cameraPosition", uniforms.cameraPosition);
    }
}

void ShaderManager::setUniform(unsigned int ordinal, const std::string &name, const glm::mat4 &m) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderManager::setUniform(const std::string &name, int value) {
    setUniform(_activeOrdinal, name, value);
}

void ShaderManager::setUniform(unsigned int ordinal, const std::string &name, int value) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform1i(loc, value);
}

void ShaderManager::setUniform(const std::string &name, float value) {
    setUniform(_activeOrdinal, name, value);
}

void ShaderManager::setUniform(unsigned int ordinal, const std::string &name, float value) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform1f(loc, value);
}

void ShaderManager::setUniform(const std::string &name, const glm::vec3 &v) {
    setUniform(_activeOrdinal, name, v);
}

void ShaderManager::setUniform(unsigned int ordinal, const std::string &name, const glm::vec3 &v) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderManager::setUniform(const std::string &name, const glm::mat4 &m) {
    setUniform(_activeOrdinal, name, m);
}

void ShaderManager::setUniform(const std::string &name, const std::vector<glm::mat4> &arr) {
    setUniform(_activeOrdinal, name, arr);
}

void ShaderManager::setUniform(unsigned int ordinal, const std::string &name, const std::vector<glm::mat4> &arr) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniformMatrix4fv(loc, arr.size(), GL_FALSE, reinterpret_cast<const GLfloat *>(&arr[0]));
}

} // namespace render

} // namespace reone
