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

#include "framebuffer.h"
#include "renderbuffer.h"
#include "texture.h"

namespace reone {

namespace graphics {

class IContext;
class IMeshRegistry;
class IShaderRegistry;
class IUniforms;

struct EnvMapDerivedRequest {
    Texture &texture;

    EnvMapDerivedRequest(Texture &texture) :
        texture(texture) {
    }
};

struct PBREnvMapTextures {
    std::shared_ptr<Texture> irradiance;
    std::shared_ptr<Texture> prefiltered;
};

class IPBRTextures {
public:
    virtual ~IPBRTextures() = default;

    virtual void refresh() = 0;
    virtual void requestEnvMapDerived(EnvMapDerivedRequest request) = 0;
    virtual Texture &brdf() = 0;
    virtual std::optional<std::reference_wrapper<PBREnvMapTextures>> findEnvMapDerived(const std::string &name) = 0;
};

class PBRTextures : public IPBRTextures, boost::noncopyable {
public:
    PBRTextures(IContext &context,
                IMeshRegistry &meshRegistry,
                IShaderRegistry &shaderRegistry,
                IUniforms &uniforms) :
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _uniforms(uniforms) {
    }

    void clear() {
        _brdf.reset();
        _envMapToDerived.clear();
        _envMapDerivedRequests.clear();
    }

    void refresh();

    void requestEnvMapDerived(EnvMapDerivedRequest request) {
        _envMapDerivedRequests.push_back(std::move(request));
    }

    Texture &brdf() {
        return *_brdf;
    }

    std::optional<std::reference_wrapper<PBREnvMapTextures>> findEnvMapDerived(const std::string &name) {
        auto it = _envMapToDerived.find(name);
        if (it == _envMapToDerived.end()) {
            return std::nullopt;
        }
        return it->second;
    }

private:
    IContext &_context;
    IMeshRegistry &_meshRegistry;
    IShaderRegistry &_shaderRegistry;
    IUniforms &_uniforms;

    std::shared_ptr<Texture> _brdf;
    std::shared_ptr<Renderbuffer> _brdfDepthBuffer;
    std::shared_ptr<Framebuffer> _brdfFramebuffer;

    std::shared_ptr<Renderbuffer> _irradianceDepthBuffer;
    std::shared_ptr<Framebuffer> _irradianceFramebuffer;

    std::vector<std::shared_ptr<Renderbuffer>> _prefilterDepthBuffers;
    std::shared_ptr<Framebuffer> _prefilterFramebuffer;

    std::map<std::string, PBREnvMapTextures> _envMapToDerived;
    std::list<EnvMapDerivedRequest> _envMapDerivedRequests;

    void refreshBRDF();
    void refreshEnvMapDerived(const EnvMapDerivedRequest &request);
    void refreshIrradiance(const EnvMapDerivedRequest &request, Texture &irradiance);
    void refreshPrefiltered(const EnvMapDerivedRequest &request, Texture &prefiltered);
};

} // namespace graphics

} // namespace reone
