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

#include "shaders.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

using namespace std;

namespace reone {

namespace render {

static const GLchar kBasicVertexShader[] = R"END(
#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec2 lightmapCoords;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec2 fragLightmapCoords;

void main() {
    gl_Position = projection * view * model * vec4(position, 1);
    fragPosition = vec3(model * vec4(position, 1));
    fragNormal = mat3(transpose(inverse(model))) * normal;
    fragTexCoords = texCoords;
    fragLightmapCoords = lightmapCoords;
}
)END";

static const GLchar kSkeletalVertexShader[] = R"END(
#version 330

const int MAX_BONES = 128;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 absTransform;
uniform mat4 absTransformInv;
uniform mat4 bones[MAX_BONES];

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 4) in vec4 boneWeights;
layout(location = 5) in vec4 boneIndices;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;

void main() {
    float weight0 = boneWeights.x;
    float weight1 = boneWeights.y;
    float weight2 = boneWeights.z;
    float weight3 = boneWeights.w;

    int index0 = int(boneIndices.x);
    int index1 = int(boneIndices.y);
    int index2 = int(boneIndices.z);
    int index3 = int(boneIndices.w);

    vec3 newPosition = vec3(0, 0, 0);
    vec4 position4 = vec4(position, 1);

    if (index0 != -1) {
        newPosition += weight0 * (absTransformInv * bones[index0] * absTransform * position4).xyz;
    }
    if (index1 != -1) {
        newPosition += weight1 * (absTransformInv * bones[index1] * absTransform * position4).xyz;
    }
    if (index2 != -1) {
        newPosition += weight2 * (absTransformInv * bones[index2] * absTransform * position4).xyz;
    }
    if (index3 != -1) {
        newPosition += weight3 * (absTransformInv * bones[index3] * absTransform * position4).xyz;
    }

    gl_Position = projection * view * model * vec4(newPosition, 1);
    fragPosition = vec3(model * vec4(position, 1));
    fragNormal = mat3(transpose(inverse(model))) * normal;
    fragTexCoords = texCoords;
}
)END";

static const GLchar kGUIVertexShader[] = R"END(
#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

out vec2 fragTexCoords;

void main() {
    gl_Position = projection * view * model * vec4(position, 1);
    fragTexCoords = texCoords;
}
)END";

static const GLchar kWhiteFragmentShader[] = R"END(
#version 330

uniform float alpha;

out vec4 color;

void main() {
    color = vec4(1, 1, 1, alpha);
}
)END";

static const GLchar kDiffuseFragmentShader[] = R"END(
#version 330

#define MAX_LIGHTS 8

uniform struct Light {
    bool ambientOnly;
    vec3 position;
    vec3 color;
} lights[MAX_LIGHTS];

uniform sampler2D diffuse;
uniform vec3 color;
uniform float alpha;
uniform int lightCount;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec3 finalColor = vec3(0.0);
    if (lightCount > 0) {
        vec3 normal = normalize(fragNormal);
        for (int i = 0; i < lightCount; ++i) {
            if (lights[i].ambientOnly) {
                finalColor += lights[i].color;
            } else {
                vec3 lightDir = normalize(lights[i].position - fragPosition);
                float brightness = max(dot(normal, lightDir), 0.0);
                finalColor += brightness * lights[i].color;
            }
        }
    } else {
        finalColor = color;
    }
    vec4 objectColor = texture(diffuse, fragTexCoords);
    fragColor = vec4(objectColor.rgb * finalColor, alpha * objectColor.a);
}
)END";

static const GLchar kDiffuseEnvmapFragmentShader[] = R"END(
#version 330

uniform sampler2D diffuse;
uniform samplerCube envmap;
uniform vec3 cameraPosition;
uniform float alpha;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 color;

void main() {
    vec3 I = normalize(fragPosition - cameraPosition);
    vec3 R = reflect(I, normalize(fragNormal));
    vec4 envmapSample = texture(envmap, R);

    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec4 objectColor = diffuseSample;
    objectColor += envmapSample * (1 - diffuseSample.a);

    color = vec4(objectColor.rgb, alpha * objectColor.a);
}
)END";

static const GLchar kDiffuseBumpyShinyFragmentShader[] = R"END(
#version 330

uniform sampler2D diffuse;
uniform samplerCube bumpyShiny;
uniform vec3 cameraPosition;
uniform float alpha;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 color;

void main() {
    vec3 I = normalize(fragPosition - cameraPosition);
    vec3 R = reflect(I, fragNormal);
    vec4 bumpyShinySample = texture(bumpyShiny, R);

    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec4 objectColor = vec4(diffuseSample.rgb, 1);
    objectColor += bumpyShinySample * (1 - diffuseSample.a);

    color = vec4(objectColor.rgb, alpha * objectColor.a);
}
)END";

static const GLchar kDiffuseLightmapFragmentShader[] = R"END(
#version 330

uniform sampler2D diffuse;
uniform sampler2D lightmap;
uniform float alpha;

in vec2 fragTexCoords;
in vec2 fragLightmapCoords;

out vec4 color;

void main() {
    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec4 lightmapSample = texture(lightmap, fragLightmapCoords);

    vec4 objectColor = vec4((diffuseSample.rgb * lightmapSample.rgb), diffuseSample.a);
    color = vec4(objectColor.rgb, alpha * objectColor.a);
}
)END";

static const GLchar kDiffuseLightmapEnvmapFragmentShader[] = R"END(
#version 330

uniform sampler2D diffuse;
uniform sampler2D lightmap;
uniform samplerCube envmap;
uniform vec3 cameraPosition;
uniform float alpha;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;

out vec4 color;

void main() {
    vec3 I = normalize(fragPosition - cameraPosition);
    vec3 R = reflect(I, normalize(fragNormal));
    vec4 envmapSample = texture(envmap, R);

    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec4 lightmapSample = texture(lightmap, fragLightmapCoords);

    vec4 objectColor = diffuseSample * lightmapSample;
    objectColor += envmapSample * (1 - diffuseSample.a);

    color = vec4(objectColor.rgb, alpha * objectColor.a);
}
)END";

static const GLchar kDiffuseLightmapBumpyShinyFragmentShader[] = R"END(
#version 330

uniform sampler2D diffuse;
uniform sampler2D lightmap;
uniform samplerCube bumpyShiny;
uniform vec3 cameraPosition;
uniform float alpha;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;

out vec4 color;

void main() {
    vec3 I = normalize(fragPosition - cameraPosition);
    vec3 R = reflect(I, fragNormal);
    vec4 bumpyShinySample = texture(bumpyShiny, R);

    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec4 lightmapSample = texture(lightmap, fragLightmapCoords);

    vec4 objectColor = diffuseSample * lightmapSample;
    objectColor += bumpyShinySample * (1 - diffuseSample.a);

    color = vec4(objectColor.rgb, alpha * objectColor.a);
}
)END";

static const GLchar kDiffuseBumpmapFragmentShader[] = R"END(
#version 330

uniform sampler2D diffuse;
uniform sampler2D bumpmap;
uniform vec3 cameraPosition;
uniform float alpha;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;

out vec4 color;

void main() {
    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec4 bumpmapSample = texture(bumpmap, fragTexCoords);

    vec4 objectColor = diffuseSample;
    color = vec4(objectColor.rgb, alpha);
}
)END";

static const GLchar kTextFragmentShader[] = R"END(
#version 330

uniform sampler2D font;
uniform vec3 textColor;

in vec2 fragTexCoords;

out vec4 color;

void main() {
    color = vec4(textColor, texture(font, fragTexCoords).a);
}
)END";

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
    initShader(ShaderName::FragmentDiffuseBumpmap, GL_FRAGMENT_SHADER, kDiffuseBumpmapFragmentShader);
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
    initProgram(ShaderProgram::SkeletalDiffuseBumpmap, ShaderName::VertexSkeletal, ShaderName::FragmentDiffuseBumpmap);
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
        throw runtime_error("Shaders: compilation failed: " + string(log, logSize));
    }

    _shaders.insert(make_pair(name, shader));
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
        throw runtime_error("Shaders: program linking failed: " + string(log, logSize));
    }

    _programs.insert(make_pair(program, ordinal));
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
        throw invalid_argument("Shaders: program not found: " + to_string(static_cast<int>(program)));
    }
    return it->second;
}

void ShaderManager::deactivate() {
    if (_activeProgram == ShaderProgram::None) return;

    glUseProgram(0);
    _activeProgram = ShaderProgram::None;
    _activeOrdinal = 0;
}

void ShaderManager::setGlobalUniforms(const ShaderUniforms &uniforms) {
    for (auto &pair : _programs) {
        activate(pair.first);

        setUniform(_activeOrdinal, "projection", uniforms.projection);
        setUniform(_activeOrdinal, "view", uniforms.view);
        setUniform(_activeOrdinal, "cameraPosition", uniforms.cameraPosition);
    }

    deactivate();
}

void ShaderManager::setUniform(unsigned int ordinal, const string &name, const glm::mat4 &m) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderManager::setUniform(const string &name, int value) {
    setUniform(_activeOrdinal, name, value);
}

void ShaderManager::setUniform(unsigned int ordinal, const string &name, int value) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform1i(loc, value);
}

void ShaderManager::setUniform(const string &name, float value) {
    setUniform(_activeOrdinal, name, value);
}

void ShaderManager::setUniform(unsigned int ordinal, const string &name, float value) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform1f(loc, value);
}

void ShaderManager::setUniform(const string &name, const glm::vec3 &v) {
    setUniform(_activeOrdinal, name, v);
}

void ShaderManager::setUniform(unsigned int ordinal, const string &name, const glm::vec3 &v) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderManager::setUniform(const string &name, const glm::mat4 &m) {
    setUniform(_activeOrdinal, name, m);
}

void ShaderManager::setUniform(const string &name, const vector<glm::mat4> &arr) {
    setUniform(_activeOrdinal, name, arr);
}

void ShaderManager::setUniform(unsigned int ordinal, const string &name, const vector<glm::mat4> &arr) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniformMatrix4fv(loc, static_cast<GLsizei>(arr.size()), GL_FALSE, reinterpret_cast<const GLfloat *>(&arr[0]));
}

} // namespace render

} // namespace reone
