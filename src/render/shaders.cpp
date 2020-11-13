/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    fragTexCoords = aTexCoords;
}
)END";

static const GLchar kModelVertexShader[] = R"END(
#version 330

const int MAX_BONES = 128;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

uniform bool uSkeletalEnabled;
uniform mat4 uAbsTransform;
uniform mat4 uAbsTransformInv;
uniform mat4 uBones[MAX_BONES];

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec2 aLightmapCoords;
layout(location = 4) in vec4 aBoneWeights;
layout(location = 5) in vec4 aBoneIndices;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec2 fragLightmapCoords;

void main() {
    vec3 newPosition = vec3(0.0);

    if (uSkeletalEnabled) {
        float weight0 = aBoneWeights.x;
        float weight1 = aBoneWeights.y;
        float weight2 = aBoneWeights.z;
        float weight3 = aBoneWeights.w;

        int index0 = int(aBoneIndices.x);
        int index1 = int(aBoneIndices.y);
        int index2 = int(aBoneIndices.z);
        int index3 = int(aBoneIndices.w);

        vec4 position4 = vec4(aPosition, 1.0);

        newPosition += weight0 * (uAbsTransformInv * uBones[index0] * uAbsTransform * position4).xyz;
        newPosition += weight1 * (uAbsTransformInv * uBones[index1] * uAbsTransform * position4).xyz;
        newPosition += weight2 * (uAbsTransformInv * uBones[index2] * uAbsTransform * position4).xyz;
        newPosition += weight3 * (uAbsTransformInv * uBones[index3] * uAbsTransform * position4).xyz;

    } else {
        newPosition = aPosition;
    }
    vec4 newPosition4 = vec4(newPosition, 1.0);

    gl_Position = uProjection * uView * uModel * newPosition4;
    fragPosition = vec3(uModel * newPosition4);
    fragNormal = mat3(transpose(inverse(uModel))) * aNormal;
    fragTexCoords = aTexCoords;
    fragLightmapCoords = aLightmapCoords;
}
)END";

static const GLchar kWhiteFragmentShader[] = R"END(
#version 330

uniform vec3 uColor;
uniform float uAlpha;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    fragColor = vec4(uColor, uAlpha);
    fragColorBright = vec4(0.0, 0.0, 0.0, 1.0);
}
)END";

static const GLchar kGUIFragmentShader[] = R"END(
#version 330

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform float uAlpha;
uniform bool uDiscardEnabled;
uniform vec3 uDiscardColor;

in vec2 fragTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void main() {
    vec4 textureSample = texture(uTexture, fragTexCoords);
    vec3 finalColor = uColor * textureSample.rgb;

    if (uDiscardEnabled && length(uDiscardColor - finalColor) < 0.01) {
        discard;
    }
    fragColor = vec4(finalColor, uAlpha * textureSample.a);
    fragColorBright = vec4(0.0, 0.0, 0.0, 1.0);
}
)END";

static const GLchar kModelFragmentShader[] = R"END(
#version 330

const int MAX_LIGHTS = 8;
const vec3 RGB_TO_LUMINOSITY = vec3(0.2126, 0.7152, 0.0722);

struct Light {
    vec3 position;
    vec3 color;
    float radius;
};

uniform bool uLightmapEnabled;
uniform bool uEnvmapEnabled;
uniform bool uBumpyShinyEnabled;
uniform bool uBumpmapEnabled;
uniform bool uSkeletalEnabled;
uniform bool uLightingEnabled;
uniform bool uSelfIllumEnabled;

uniform sampler2D uDiffuse;
uniform sampler2D uLightmap;
uniform sampler2D uBumpmap;
uniform samplerCube uEnvmap;
uniform samplerCube uBumpyShiny;

uniform vec3 uCameraPosition;
uniform float uAlpha;

uniform int uLightCount;
uniform vec3 uAmbientLightColor;
uniform vec3 uSelfIllumColor;
uniform Light uLights[MAX_LIGHTS];

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec2 fragLightmapCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColorBright;

void applyLightmap(inout vec3 color) {
    vec4 lightmapSample = texture(uLightmap, fragLightmapCoords);
    color *= lightmapSample.rgb;
}

void applyEnvmap(samplerCube image, vec3 normal, float a, inout vec3 color) {
    vec3 I = normalize(fragPosition - uCameraPosition);
    vec3 R = reflect(I, normal);
    vec4 sample = texture(image, R);

    color += sample.rgb * a;
}

void applyLighting(vec3 normal, inout vec3 color) {
    color += uAmbientLightColor;

    if (uLightCount == 0) return;

    for (int i = 0; i < uLightCount; ++i) {
        vec3 surfaceToLight = uLights[i].position - fragPosition;
        vec3 lightDir = normalize(surfaceToLight);

        vec3 surfaceToCamera = normalize(uCameraPosition - fragPosition);
        float diffuseCoeff = max(dot(normal, lightDir), 0.0);
        float specularCoeff = 0.0;

        if (diffuseCoeff > 0.0) {
            specularCoeff = 0.25 * pow(max(0.0, dot(surfaceToCamera, reflect(-lightDir, normal))), 32);
        }
        float distToLight = length(surfaceToLight);

        float attenuation = clamp(1.0 - distToLight / uLights[i].radius, 0.0, 1.0);
        attenuation *= attenuation;

        color += attenuation * (diffuseCoeff + specularCoeff) * uLights[i].color;
    }
}

void main() {
    vec4 diffuseSample = texture(uDiffuse, fragTexCoords);
    vec3 surfaceColor = diffuseSample.rgb;
    vec3 lightColor = vec3(0.0);
    vec3 normal = normalize(fragNormal);

    if (uLightmapEnabled) {
        applyLightmap(surfaceColor);
    }
    if (uEnvmapEnabled) {
        applyEnvmap(uEnvmap, normal, 1.0 - diffuseSample.a, surfaceColor);
    } else if (uBumpyShinyEnabled) {
        applyEnvmap(uBumpyShiny, normal, 1.0 - diffuseSample.a, surfaceColor);
    }
    if (uLightingEnabled) {
        applyLighting(normal, lightColor);
        lightColor = min(lightColor, 1.0);
    } else {
        lightColor = vec3(1.0);
    }
    float finalAlpha = uAlpha;

    if (!uEnvmapEnabled && !uBumpyShinyEnabled && !uBumpmapEnabled) {
        finalAlpha *= diffuseSample.a;
    }
    fragColor = vec4(lightColor * surfaceColor, finalAlpha);

    if (uSelfIllumEnabled) {
        vec3 color = uSelfIllumColor * diffuseSample.rgb;
        fragColorBright = vec4(smoothstep(0.75, 1.0, color), 1.0);
    } else {
        fragColorBright = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
)END";

static const GLchar kGaussianBlurFragmentShader[] = R"END(
#version 330

uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform vec2 uDirection;

out vec4 fragColor;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy / uResolution);
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * uDirection;
    vec2 off2 = vec2(3.2307692308) * uDirection;
    color += texture2D(uTexture, uv) * 0.2270270270;
    color += texture2D(uTexture, uv + (off1 / uResolution)) * 0.3162162162;
    color += texture2D(uTexture, uv - (off1 / uResolution)) * 0.3162162162;
    color += texture2D(uTexture, uv + (off2 / uResolution)) * 0.0702702703;
    color += texture2D(uTexture, uv - (off2 / uResolution)) * 0.0702702703;

    fragColor = color;
}
)END";

static const GLchar kBloomFragmentShader[] = R"END(
#version 330

uniform sampler2D uGeometry;
uniform sampler2D uBloom;

in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
    vec3 geometryColor = texture(uGeometry, fragTexCoords).rgb;
    vec3 bloomColor = texture(uBloom, fragTexCoords).rgb;

    fragColor = vec4(geometryColor + bloomColor, 1.0);
}
)END";

Shaders &Shaders::instance() {
    static Shaders instance;
    return instance;
}

void Shaders::initGL() {
    initShader(ShaderName::VertexGUI, GL_VERTEX_SHADER, kGUIVertexShader);
    initShader(ShaderName::VertexModel, GL_VERTEX_SHADER, kModelVertexShader);
    initShader(ShaderName::FragmentWhite, GL_FRAGMENT_SHADER, kWhiteFragmentShader);
    initShader(ShaderName::FragmentGUI, GL_FRAGMENT_SHADER, kGUIFragmentShader);
    initShader(ShaderName::FragmentModel, GL_FRAGMENT_SHADER, kModelFragmentShader);
    initShader(ShaderName::FragmentBlur, GL_FRAGMENT_SHADER, kGaussianBlurFragmentShader);
    initShader(ShaderName::FragmentBloom, GL_FRAGMENT_SHADER, kBloomFragmentShader);

    initProgram(ShaderProgram::GUIGUI, ShaderName::VertexGUI, ShaderName::FragmentGUI);
    initProgram(ShaderProgram::GUIBlur, ShaderName::VertexGUI, ShaderName::FragmentBlur);
    initProgram(ShaderProgram::GUIBloom, ShaderName::VertexGUI, ShaderName::FragmentBloom);
    initProgram(ShaderProgram::GUIWhite, ShaderName::VertexGUI, ShaderName::FragmentWhite);
    initProgram(ShaderProgram::ModelWhite, ShaderName::VertexModel, ShaderName::FragmentWhite);
    initProgram(ShaderProgram::ModelModel, ShaderName::VertexModel, ShaderName::FragmentModel);
}

void Shaders::initShader(ShaderName name, unsigned int type, const char *source) {
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

void Shaders::initProgram(ShaderProgram program, ShaderName vertexShader, ShaderName fragmentShader) {
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

Shaders::~Shaders() {
    deinitGL();
}

void Shaders::deinitGL() {
    for (auto &pair :_programs) {
        glDeleteProgram(pair.second);
    }
    _programs.clear();

    for (auto &pair : _shaders) {
        glDeleteShader(pair.second);
    }
    _shaders.clear();
}

void Shaders::activate(ShaderProgram program, const LocalUniforms &locals) {
    if (_activeProgram != program) {
        unsigned int ordinal = getOrdinal(program);
        glUseProgram(ordinal);

        _activeProgram = program;
        _activeOrdinal = ordinal;
    }
    setLocalUniforms(locals);
}

unsigned int Shaders::getOrdinal(ShaderProgram program) const {
    auto it = _programs.find(program);
    if (it == _programs.end()) {
        throw invalid_argument("Shaders: program not found: " + to_string(static_cast<int>(program)));
    }
    return it->second;
}

static const string &getLightUniformName(int index, const char *propName) {
    static unordered_map<int, unordered_map<const char *, string>> cache;
    auto &cacheByIndex = cache[index];
    auto maybeName = cacheByIndex.find(propName);

    if (maybeName != cacheByIndex.end()) {
        return maybeName->second;
    }
    string name(str(boost::format("uLights[%d].%s") % index % propName));
    auto pair = cacheByIndex.insert(make_pair(propName, name));

    return pair.first->second;
}

void Shaders::setLocalUniforms(const LocalUniforms &locals) {
    setUniform("uModel", locals.model);
    setUniform("uColor", locals.color);
    setUniform("uAlpha", locals.alpha);
    setUniform("uLightmapEnabled", locals.features.lightmapEnabled);
    setUniform("uEnvmapEnabled", locals.features.envmapEnabled);
    setUniform("uLightmapEnabled", locals.features.lightmapEnabled);
    setUniform("uBumpyShinyEnabled", locals.features.bumpyShinyEnabled);
    setUniform("uBumpmapEnabled", locals.features.bumpmapEnabled);
    setUniform("uSkeletalEnabled", locals.features.skeletalEnabled);
    setUniform("uLightingEnabled", locals.features.lightingEnabled);
    setUniform("uSelfIllumEnabled", locals.features.selfIllumEnabled);
    setUniform("uDiscardEnabled", locals.features.discardEnabled);
    setUniform("uLightmap", locals.textures.lightmap);
    setUniform("uEnvmap", locals.textures.envmap);
    setUniform("uBumpyShiny", locals.textures.bumpyShiny);
    setUniform("uBumpmap", locals.textures.bumpmap);
    setUniform("uBloom", locals.textures.bloom);

    if (locals.features.skeletalEnabled) {
        setUniform("uAbsTransform", locals.skeletal.absTransform);
        setUniform("uAbsTransformInv", locals.skeletal.absTransformInv);
        setUniform("uBones", locals.skeletal.bones);
    }
    if (locals.features.lightingEnabled) {
        int lightCount = static_cast<int>(locals.lighting.lights.size());
        setUniform("uLightCount", lightCount);
        setUniform("uAmbientLightColor", locals.lighting.ambientColor);

        for (int i = 0; i < lightCount; ++i) {
            const ShaderLight &light = locals.lighting.lights[i];
            setUniform(getLightUniformName(i, "position"), light.position);
            setUniform(getLightUniformName(i, "color"), light.color);
            setUniform(getLightUniformName(i, "radius"), light.radius);
        }
    }
    if (locals.features.selfIllumEnabled) {
        setUniform("uSelfIllumColor", locals.selfIllumColor);
    }
    if (locals.features.blurEnabled) {
        setUniform("uResolution", locals.blur.resolution);
        setUniform("uDirection", locals.blur.direction);
    }
    if (locals.features.discardEnabled) {
        setUniform("uDiscardColor", locals.discardColor);
    }
}

void Shaders::setUniform(const string &name, const glm::mat4 &m) {
    setUniform(name, [this, &m](int loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    });
}

void Shaders::setUniform(const string &name, const function<void(int)> &setter) {
    static unordered_map<uint32_t, unordered_map<string, GLint>> locsByProgram;

    unordered_map<string, GLint> &locs = locsByProgram[_activeOrdinal];
    auto maybeLoc = locs.find(name);
    GLint loc = 0;

    if (maybeLoc != locs.end()) {
        loc = maybeLoc->second;
    } else {
        loc = glGetUniformLocation(_activeOrdinal, name.c_str());
        locs.insert(make_pair(name, loc));
    }
    if (loc != -1) {
        setter(loc);
    }
}

void Shaders::setUniform(const string &name, int value) {
    setUniform(name, [this, &value](int loc) {
        glUniform1i(loc, value);
    });
}

void Shaders::setUniform(const string &name, float value) {
    setUniform(name, [this, &value](int loc) {
        glUniform1f(loc, value);
    });
}

void Shaders::setUniform(const string &name, const glm::vec2 &v) {
    setUniform(name, [this, &v](int loc) {
        glUniform2f(loc, v.x, v.y);
    });
}

void Shaders::setUniform(const string &name, const glm::vec3 &v) {
    setUniform(name, [this, &v](int loc) {
        glUniform3f(loc, v.x, v.y, v.z);
    });
}

void Shaders::setUniform(const string &name, const vector<glm::mat4> &arr) {
    setUniform(name, [this, &arr](int loc) {
        glUniformMatrix4fv(loc, static_cast<GLsizei>(arr.size()), GL_FALSE, reinterpret_cast<const GLfloat *>(&arr[0]));
    });
}

void Shaders::deactivate() {
    if (_activeProgram == ShaderProgram::None) return;

    glUseProgram(0);
    _activeProgram = ShaderProgram::None;
    _activeOrdinal = 0;
}

void Shaders::setGlobalUniforms(const GlobalUniforms &globals) {
    uint32_t ordinal = _activeOrdinal;

    for (auto &program : _programs) {
        glUseProgram(program.second);
        _activeOrdinal = program.second;

        setUniform("uProjection", globals.projection);
        setUniform("uView", globals.view);
        setUniform("uCameraPosition", globals.cameraPosition);
    }
    glUseProgram(ordinal);
    _activeOrdinal = ordinal;
}

} // namespace render

} // namespace reone
