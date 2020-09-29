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

#include <boost/format.hpp>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

using namespace std;

namespace reone {

namespace render {

static const GLchar kGUIVertexShader[] = R"END(
#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

out vec2 fragTexCoords;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    fragTexCoords = texCoords;
}
)END";

static const GLchar kModelVertexShader[] = R"END(
#version 330

const int MAX_BONES = 128;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool skeletalEnabled;
uniform mat4 absTransform;
uniform mat4 absTransformInv;
uniform mat4 bones[MAX_BONES];

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec2 lightmapCoords;
layout(location = 4) in vec4 boneWeights;
layout(location = 5) in vec4 boneIndices;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec2 fragLightmapCoords;

void main() {
    vec3 newPosition = vec3(0.0);

    if (skeletalEnabled) {
        float weight0 = boneWeights.x;
        float weight1 = boneWeights.y;
        float weight2 = boneWeights.z;
        float weight3 = boneWeights.w;

        int index0 = int(boneIndices.x);
        int index1 = int(boneIndices.y);
        int index2 = int(boneIndices.z);
        int index3 = int(boneIndices.w);

        vec4 position4 = vec4(position, 1.0);

        newPosition += weight0 * (absTransformInv * bones[index0] * absTransform * position4).xyz;
        newPosition += weight1 * (absTransformInv * bones[index1] * absTransform * position4).xyz;
        newPosition += weight2 * (absTransformInv * bones[index2] * absTransform * position4).xyz;
        newPosition += weight3 * (absTransformInv * bones[index3] * absTransform * position4).xyz;

    } else {
        newPosition = position;
    }

    vec4 newPosition4 = vec4(newPosition, 1.0);

    gl_Position = projection * view * model * newPosition4;
    fragPosition = vec3(model * newPosition4);
    fragNormal = mat3(transpose(inverse(model))) * normal;
    fragTexCoords = texCoords;
    fragLightmapCoords = lightmapCoords;
}
)END";

static const GLchar kWhiteFragmentShader[] = R"END(
#version 330

uniform float alpha;

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0, 1.0, 1.0, alpha);
}
)END";

static const GLchar kGUIFragmentShader[] = R"END(
#version 330

uniform sampler2D image;
uniform vec3 color;
uniform float alpha;

in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec4 imageSample = texture(image, fragTexCoords);
    fragColor = vec4(color * imageSample.rgb, alpha * imageSample.a);
}
)END";

static const GLchar kModelFragmentShader[] = R"END(
#version 330

const int MAX_LIGHTS = 8;
const vec3 RGB_TO_LUMINOSITY = vec3(0.2126, 0.7152, 0.0722);

uniform struct Light {
    vec3 position;
    float radius;
    vec3 color;
    float multiplier;
} lights[MAX_LIGHTS];

uniform bool lightmapEnabled;
uniform bool envmapEnabled;
uniform bool bumpyShinyEnabled;
uniform bool bumpmapEnabled;
uniform bool lightingEnabled;
uniform bool selfIllumEnabled;

uniform sampler2D diffuse;
uniform sampler2D lightmap;
uniform sampler2D bumpmap;
uniform samplerCube envmap;
uniform samplerCube bumpyShiny;

uniform vec3 cameraPosition;
uniform float alpha;

uniform int lightCount;
uniform vec3 ambientLightColor;
uniform vec3 selfIllumColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;

out vec4 fragColor;

void applyLightmap(inout vec3 color) {
    vec4 lightmapSample = texture(lightmap, fragLightmapCoords);
    color *= lightmapSample.rgb;
}

void applyEnvmap(samplerCube image, vec3 normal, float a, inout vec3 color) {
    vec3 I = normalize(fragPosition - cameraPosition);
    vec3 R = reflect(I, normal);
    vec4 sample = texture(image, R);

    color += sample.rgb * a;
}

void applyLighting(vec3 normal, inout vec3 color) {
    color += ambientLightColor;

    if (lightCount == 0) return;

    for (int i = 0; i < lightCount; ++i) {
        vec3 surfaceToLight = lights[i].position - fragPosition;
        vec3 lightDir = normalize(surfaceToLight);

        vec3 surfaceToCamera = normalize(cameraPosition - fragPosition);
        float diffuseCoeff = max(dot(normal, lightDir), 0.0);
        float specularCoeff = 0.0;

        if (diffuseCoeff > 0.0) {
            specularCoeff = 0.25 * pow(max(0.0, dot(surfaceToCamera, reflect(-lightDir, normal))), 32);
        }
        float distToLight = length(surfaceToLight);

        float attenuation = clamp(1.0 - distToLight / lights[i].radius, 0.0, 1.0);
        attenuation *= attenuation;

        color += attenuation * (diffuseCoeff + specularCoeff) * lights[i].color;
    }
}

void applySelfIllum(inout vec3 color) {
    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    float luminosity = dot(diffuseSample.rgb, RGB_TO_LUMINOSITY);

    color += 0.5 * smoothstep(0.5, 1.0, luminosity) * selfIllumColor;
}

void main() {
    vec4 diffuseSample = texture(diffuse, fragTexCoords);
    vec3 surfaceColor = diffuseSample.rgb;
    vec3 lightColor = vec3(0.0);
    vec3 normal = normalize(fragNormal);

    if (lightmapEnabled) {
        applyLightmap(surfaceColor);
    }
    if (envmapEnabled) {
        applyEnvmap(envmap, normal, 1.0 - diffuseSample.a, surfaceColor);
    } else if (bumpyShinyEnabled) {
        applyEnvmap(bumpyShiny, normal, 1.0 - diffuseSample.a, surfaceColor);
    }
    if (lightingEnabled) {
        applyLighting(normal, lightColor);
        lightColor = min(lightColor, 1.0);
    } else {
        lightColor = vec3(1.0);
    }
    if (selfIllumEnabled) {
        applySelfIllum(lightColor);
    }
    float finalAlpha = alpha;

    if (!envmapEnabled && !bumpyShinyEnabled) {
        finalAlpha *= diffuseSample.a;
    }
    fragColor = vec4(lightColor * surfaceColor, finalAlpha);
}
)END";

static const GLchar kGaussianBlurFragmentShader[] = R"END(
#version 330

uniform sampler2D image;
uniform vec2 resolution;
uniform vec2 direction;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy / resolution);
    vec2 off1 = vec2(1.3333333333333333) * direction;

    vec4 color = vec4(0.0);
    color += texture2D(image, uv) * 0.29411764705882354;
    color += texture2D(image, uv + (off1 / resolution)) * 0.35294117647058826;
    color += texture2D(image, uv - (off1 / resolution)) * 0.35294117647058826;

    fragColor = color;
}
)END";

ShaderManager &ShaderManager::instance() {
    static ShaderManager instance;
    return instance;
}

void ShaderManager::initGL() {
    initShader(ShaderName::VertexGUI, GL_VERTEX_SHADER, kGUIVertexShader);
    initShader(ShaderName::VertexModel, GL_VERTEX_SHADER, kModelVertexShader);
    initShader(ShaderName::FragmentWhite, GL_FRAGMENT_SHADER, kWhiteFragmentShader);
    initShader(ShaderName::FragmentGUI, GL_FRAGMENT_SHADER, kGUIFragmentShader);
    initShader(ShaderName::FragmentModel, GL_FRAGMENT_SHADER, kModelFragmentShader);
    initShader(ShaderName::FragmentGaussianBlur, GL_FRAGMENT_SHADER, kGaussianBlurFragmentShader);

    initProgram(ShaderProgram::GUIGUI, ShaderName::VertexGUI, ShaderName::FragmentGUI);
    initProgram(ShaderProgram::ModelWhite, ShaderName::VertexModel, ShaderName::FragmentWhite);
    initProgram(ShaderProgram::ModelModel, ShaderName::VertexModel, ShaderName::FragmentModel);
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
        throw runtime_error(str(boost::format("Shader %d compilation failed: %s") % static_cast<int>(name) % string(log, logSize)));
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

void ShaderManager::setUniform(const string &name, const glm::vec2 &v) {
    setUniform(_activeOrdinal, name, v);
}

void ShaderManager::setUniform(unsigned int ordinal, const string &name, const glm::vec2 &v) {
    GLint loc = glGetUniformLocation(ordinal, name.c_str());
    if (loc == -1) return;

    glUniform2f(loc, v.x, v.y);
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
