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

#include "shaders.h"

#include <cstdarg>
#include <stdexcept>

#include <boost/format.hpp>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "texture.h"
#include "textures.h"

using namespace std;

namespace reone {

namespace graphics {

extern char g_shaderBaseHeader[];
extern char g_shaderBaseModel[];
extern char g_shaderBasePBR[];
extern char g_shaderBasePBRIBL[];
extern char g_shaderVertexSimple[];
extern char g_shaderVertexModel[];
extern char g_shaderVertexParticle[];
extern char g_shaderVertexGrass[];
extern char g_shaderVertexText[];
extern char g_shaderVertexBillboard[];
extern char g_shaderGeometryDepth[];
extern char g_shaderFragmentColor[];
extern char g_shaderFragmentDepth[];
extern char g_shaderFragmentGUI[];
extern char g_shaderFragmentText[];
extern char g_shaderFragmentBlinnPhong[];
extern char g_shaderFragmentPBR[];
extern char g_shaderFragmentParticle[];
extern char g_shaderFragmentGrass[];
extern char g_shaderFragmentIrradiance[];
extern char g_shaderFragmentPrefilter[];
extern char g_shaderFragmentBRDF[];
extern char g_shaderFragmentBlur[];
extern char g_shaderFragmentPresentWorld[];

static constexpr int kBindingPointIndexCombined = 1;
static constexpr int kBindingPointIndexText = 2;
static constexpr int kBindingPointIndexLighting = 3;
static constexpr int kBindingPointIndexSkeletal = 4;
static constexpr int kBindingPointIndexParticles = 5;
static constexpr int kBindingPointIndexGrass = 6;
static constexpr int kBindingPointIndexDanglymesh = 7;

Shaders &Shaders::instance() {
    static Shaders instance;
    return instance;
}

void Shaders::init() {
    if (_inited) return;

    initShader(ShaderName::VertexSimple, GL_VERTEX_SHADER, { g_shaderBaseHeader, g_shaderVertexSimple });
    initShader(ShaderName::VertexModel, GL_VERTEX_SHADER, { g_shaderBaseHeader, g_shaderVertexModel });
    initShader(ShaderName::VertexParticle, GL_VERTEX_SHADER, { g_shaderBaseHeader, g_shaderVertexParticle });
    initShader(ShaderName::VertexGrass, GL_VERTEX_SHADER, { g_shaderBaseHeader, g_shaderVertexGrass });
    initShader(ShaderName::VertexText, GL_VERTEX_SHADER, { g_shaderBaseHeader, g_shaderVertexText });
    initShader(ShaderName::VertexBillboard, GL_VERTEX_SHADER, { g_shaderBaseHeader, g_shaderVertexBillboard });
    initShader(ShaderName::GeometryDepth, GL_GEOMETRY_SHADER, { g_shaderBaseHeader, g_shaderGeometryDepth });
    initShader(ShaderName::FragmentColor, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentColor });
    initShader(ShaderName::FragmentDepth, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentDepth });
    initShader(ShaderName::FragmentGUI, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentGUI });
    initShader(ShaderName::FragmentText, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentText });
    initShader(ShaderName::FragmentBlinnPhong, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderBaseModel, g_shaderFragmentBlinnPhong });
    initShader(ShaderName::FragmentPBR, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderBasePBR, g_shaderBasePBRIBL, g_shaderBaseModel, g_shaderFragmentPBR });
    initShader(ShaderName::FragmentParticle, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentParticle });
    initShader(ShaderName::FragmentGrass, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentGrass });
    initShader(ShaderName::FragmentIrradiance, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentIrradiance });
    initShader(ShaderName::FragmentPrefilter, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderBasePBR, g_shaderBasePBRIBL, g_shaderFragmentPrefilter });
    initShader(ShaderName::FragmentBRDF, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderBasePBR, g_shaderBasePBRIBL, g_shaderFragmentBRDF });
    initShader(ShaderName::FragmentBlur, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentBlur });
    initShader(ShaderName::FragmentPresentWorld, GL_FRAGMENT_SHADER, { g_shaderBaseHeader, g_shaderFragmentPresentWorld });

    initProgram(ShaderProgram::SimpleColor, { ShaderName::VertexSimple, ShaderName::FragmentColor });
    initProgram(ShaderProgram::SimpleDepth, { ShaderName::VertexSimple, ShaderName::GeometryDepth, ShaderName::FragmentDepth });
    initProgram(ShaderProgram::SimpleGUI, { ShaderName::VertexSimple, ShaderName::FragmentGUI });
    initProgram(ShaderProgram::SimpleIrradiance, { ShaderName::VertexSimple, ShaderName::FragmentIrradiance });
    initProgram(ShaderProgram::SimplePrefilter, { ShaderName::VertexSimple, ShaderName::FragmentPrefilter });
    initProgram(ShaderProgram::SimpleBRDF, { ShaderName::VertexSimple, ShaderName::FragmentBRDF });
    initProgram(ShaderProgram::SimpleBlur, { ShaderName::VertexSimple, ShaderName::FragmentBlur });
    initProgram(ShaderProgram::SimplePresentWorld, { ShaderName::VertexSimple, ShaderName::FragmentPresentWorld });
    initProgram(ShaderProgram::ModelColor, { ShaderName::VertexModel, ShaderName::FragmentColor });
    initProgram(ShaderProgram::ModelBlinnPhong, { ShaderName::VertexModel, ShaderName::FragmentBlinnPhong });
    initProgram(ShaderProgram::ModelPBR, { ShaderName::VertexModel, ShaderName::FragmentPBR });
    initProgram(ShaderProgram::ParticleParticle, { ShaderName::VertexParticle, ShaderName::FragmentParticle });
    initProgram(ShaderProgram::GrassGrass, { ShaderName::VertexGrass, ShaderName::FragmentGrass });
    initProgram(ShaderProgram::TextText, { ShaderName::VertexText, ShaderName::FragmentText });
    initProgram(ShaderProgram::BillboardGUI, { ShaderName::VertexBillboard, ShaderName::FragmentGUI });

    glGenBuffers(1, &_uboCombined);
    glGenBuffers(1, &_uboText);
    glGenBuffers(1, &_uboLighting);
    glGenBuffers(1, &_uboSkeletal);
    glGenBuffers(1, &_uboParticles);
    glGenBuffers(1, &_uboGrass);
    glGenBuffers(1, &_uboDanglymesh);

    for (auto &program : _programs) {
        glUseProgram(program.second);
        _activeOrdinal = program.second;

        initUBOs();
        initTextureUniforms();

        _activeOrdinal = 0;
        glUseProgram(0);
    }

    _defaultUniforms.text = make_shared<TextUniforms>();
    _defaultUniforms.lighting = make_shared<LightingUniforms>();
    _defaultUniforms.skeletal = make_shared<SkeletalUniforms>();
    _defaultUniforms.particles = make_shared<ParticlesUniforms>();
    _defaultUniforms.grass = make_shared<GrassUniforms>();
    _defaultUniforms.danglymesh = make_shared<DanglymeshUniforms>();

    _inited = true;
}

void Shaders::initShader(ShaderName name, unsigned int type, vector<const char *> sources) {
    GLuint shader = glCreateShader(type);
    GLint success;
    char log[512];
    GLsizei logSize;

    glShaderSource(shader, static_cast<GLsizei>(sources.size()), &sources[0], nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(log), &logSize, log);
        throw runtime_error(str(boost::format("Shader %d compilation failed: %s") % static_cast<int>(name) % string(log, logSize)));
    }

    _shaders.insert(make_pair(name, shader));
}

void Shaders::initProgram(ShaderProgram program, vector<ShaderName> shaders) {
    GLuint ordinal = glCreateProgram();

    for (auto &shader : shaders) {
        glAttachShader(ordinal, _shaders.find(shader)->second);
    }
    glLinkProgram(ordinal);

    GLint success;
    char log[512];
    GLsizei logSize;

    glGetProgramiv(ordinal, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ordinal, sizeof(log), &logSize, log);
        throw runtime_error("Shaders: program linking failed: " + string(log, logSize));
    }

    _programs.insert(make_pair(program, ordinal));
}

void Shaders::initUBOs() {
    static ShaderUniforms defaultsCombined;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    static DanglymeshUniforms defaultsDanglymesh;

    initUBO("Combined", kBindingPointIndexCombined, _uboCombined, defaultsCombined, offsetof(ShaderUniforms, text));
    initUBO("Text", kBindingPointIndexText, _uboText, defaultsText);
    initUBO("Lighting", kBindingPointIndexLighting, _uboLighting, defaultsLighting);
    initUBO("Skeletal", kBindingPointIndexSkeletal, _uboSkeletal, defaultsSkeletal);
    initUBO("Particles", kBindingPointIndexParticles, _uboParticles, defaultsParticles);
    initUBO("Grass", kBindingPointIndexGrass, _uboGrass, defaultsGrass);
    initUBO("Danglymesh", kBindingPointIndexDanglymesh, _uboDanglymesh, defaultsDanglymesh);
}

template <class T>
void Shaders::initUBO(const string &block, int bindingPoint, uint32_t ubo, const T &defaults, size_t size) {
    uint32_t blockIdx = glGetUniformBlockIndex(_activeOrdinal, block.c_str());
    if (blockIdx != GL_INVALID_INDEX) {
        glUniformBlockBinding(_activeOrdinal, blockIdx, bindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, &defaults, GL_STATIC_DRAW);
    }
}

void Shaders::initTextureUniforms() {
    setUniform("uDiffuse", TextureUnits::diffuse);
    setUniform("uLightmap", TextureUnits::lightmap);
    setUniform("uEnvmap", TextureUnits::envmap);
    setUniform("uBumpmap", TextureUnits::bumpmap);
    setUniform("uBloom", TextureUnits::bloom);
    setUniform("uIrradianceMap", TextureUnits::irradianceMap);
    setUniform("uPrefilterMap", TextureUnits::prefilterMap);
    setUniform("uBRDFLookup", TextureUnits::brdfLookup);
    setUniform("uShadowMap", TextureUnits::shadowMap);
    setUniform("uCubeShadowMap", TextureUnits::cubeShadowMap);
}

Shaders::~Shaders() {
    deinit();
}

void Shaders::deinit() {
    if (!_inited) return;

    // Delete UBO
    if (_uboCombined) {
        glDeleteBuffers(1, &_uboCombined);
        _uboCombined = 0;
    }
    if (_uboText) {
        glDeleteBuffers(1, &_uboText);
        _uboText = 0;
    }
    if (_uboLighting) {
        glDeleteBuffers(1, &_uboLighting);
        _uboLighting = 0;
    }
    if (_uboSkeletal) {
        glDeleteBuffers(1, &_uboSkeletal);
        _uboSkeletal = 0;
    }
    if (_uboParticles) {
        glDeleteBuffers(1, &_uboParticles);
        _uboParticles = 0;
    }
    if (_uboGrass) {
        glDeleteBuffers(1, &_uboGrass);
        _uboGrass = 0;
    }
    if (_uboDanglymesh) {
        glDeleteBuffers(1, &_uboDanglymesh);
        _uboDanglymesh = 0;
    }

    // Delete programs
    for (auto &pair : _programs) {
        glDeleteProgram(pair.second);
    }
    _programs.clear();

    // Delete shaders
    for (auto &pair : _shaders) {
        glDeleteShader(pair.second);
    }
    _shaders.clear();

    _inited = false;
}

void Shaders::activate(ShaderProgram program, const ShaderUniforms &uniforms) {
    if (_activeProgram != program) {
        unsigned int ordinal = getOrdinal(program);
        glUseProgram(ordinal);

        _activeProgram = program;
        _activeOrdinal = ordinal;
    }
    setUniforms(uniforms);
}

unsigned int Shaders::getOrdinal(ShaderProgram program) const {
    auto it = _programs.find(program);
    if (it == _programs.end()) {
        throw invalid_argument("Shaders: program not found: " + to_string(static_cast<int>(program)));
    }
    return it->second;
}

void Shaders::setUniforms(const ShaderUniforms &uniforms) {
    glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexCombined, _uboCombined);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CombinedUniforms), &uniforms.combined);

    if (uniforms.combined.featureMask & UniformFeatureFlags::text) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexText, _uboText);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TextUniforms), uniforms.text.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::lighting) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexLighting, _uboLighting);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingUniforms), uniforms.lighting.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::skeletal) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexSkeletal, _uboSkeletal);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SkeletalUniforms), uniforms.skeletal.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::particles) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexParticles, _uboParticles);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ParticlesUniforms), uniforms.particles.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::grass) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexGrass, _uboGrass);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GrassUniforms), uniforms.grass.get());
    }
    if (uniforms.combined.featureMask & UniformFeatureFlags::danglymesh) {
        glBindBufferBase(GL_UNIFORM_BUFFER, kBindingPointIndexDanglymesh, _uboDanglymesh);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DanglymeshUniforms), uniforms.danglymesh.get());
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

} // namespace graphics

} // namespace reone
