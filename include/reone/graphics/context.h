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

#include "options.h"
#include "types.h"

namespace reone {

namespace graphics {

class ShaderProgram;
class Texture;

class IGraphicsContext {
public:
    virtual ~IGraphicsContext() = default;

    virtual void clearColor(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;
    virtual void clearDepth() = 0;
    virtual void clearColorDepth(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;

    virtual void bind(Texture &texture, int unit = TextureUnits::mainTex) = 0;
    virtual void bindBuiltInTextures() = 0;

    virtual void useProgram(ShaderProgram &program) = 0;
    virtual void resetProgram() = 0;

    virtual void withDepthTest(DepthTestMode mode, const std::function<void()> &block) = 0;
    virtual void withFaceCulling(CullFaceMode mode, const std::function<void()> &block) = 0;
    virtual void withBlending(BlendMode mode, const std::function<void()> &block) = 0;
    virtual void withPolygonMode(PolygonMode mode, const std::function<void()> &block) = 0;
    virtual void withViewport(glm::ivec4 viewport, const std::function<void()> &block) = 0;
    virtual void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block) = 0;

    // Built-in

    virtual std::shared_ptr<graphics::Texture> default2DRGB() const = 0;
    virtual std::shared_ptr<graphics::Texture> defaultArrayDepth() const = 0;
    virtual std::shared_ptr<graphics::Texture> defaultCubemapRGB() const = 0;
    virtual std::shared_ptr<graphics::Texture> defaultCubemapDepth() const = 0;

    virtual std::shared_ptr<graphics::Texture> noiseRG() const = 0;
    virtual std::shared_ptr<graphics::Texture> ssaoRGB() const = 0;
    virtual std::shared_ptr<graphics::Texture> ssrRGBA() const = 0;

    // END Built-in
};

class GraphicsContext : public IGraphicsContext, boost::noncopyable {
public:
    GraphicsContext(GraphicsOptions &options) :
        _options(options) {
    }

    void init();

    void clearColor(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) override;
    void clearDepth() override;
    void clearColorDepth(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) override;

    void bind(Texture &texture, int unit = TextureUnits::mainTex) override;
    void bindBuiltInTextures() override;

    void useProgram(ShaderProgram &program) override;
    void resetProgram() override;

    void withDepthTest(DepthTestMode mode, const std::function<void()> &block) override;
    void withFaceCulling(CullFaceMode mode, const std::function<void()> &block) override;
    void withBlending(BlendMode mode, const std::function<void()> &block) override;
    void withPolygonMode(PolygonMode mode, const std::function<void()> &block) override;
    void withViewport(glm::ivec4 viewport, const std::function<void()> &block) override;
    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block) override;

    // Built-in textures

    std::shared_ptr<graphics::Texture> default2DRGB() const override { return _default2DRGB; }
    std::shared_ptr<graphics::Texture> defaultArrayDepth() const override { return _defaultArrayDepth; }
    std::shared_ptr<graphics::Texture> defaultCubemapRGB() const override { return _defaultCubemapRGB; }
    std::shared_ptr<graphics::Texture> defaultCubemapDepth() const override { return _defaultCubemapDepth; }

    std::shared_ptr<graphics::Texture> noiseRG() const override { return _noiseRG; }
    std::shared_ptr<graphics::Texture> ssaoRGB() const override { return _ssaoRGB; }
    std::shared_ptr<graphics::Texture> ssrRGBA() const override { return _ssrRGBA; }

    // END Built-in textures

private:
    GraphicsOptions &_options;

    bool _inited {false};
    int _activeTexUnit {0};
    ShaderProgram *_usedProgram {nullptr};

    // States

    std::stack<DepthTestMode> _depthTestModes;
    std::stack<CullFaceMode> _cullFaceModes;
    std::stack<BlendMode> _blendModes;
    std::stack<PolygonMode> _polygonModes;
    std::stack<glm::ivec4> _viewports;

    // END States

    // Built-in textures

    std::shared_ptr<graphics::Texture> _default2DRGB;
    std::shared_ptr<graphics::Texture> _defaultArrayDepth;
    std::shared_ptr<graphics::Texture> _defaultCubemapRGB;
    std::shared_ptr<graphics::Texture> _defaultCubemapDepth;

    std::shared_ptr<graphics::Texture> _noiseRG;
    std::shared_ptr<graphics::Texture> _ssaoRGB;
    std::shared_ptr<graphics::Texture> _ssrRGBA;

    // END Built-in textures

    void setDepthTestMode(DepthTestMode mode);
    void setCullFaceMode(CullFaceMode mode);
    void setBlendMode(BlendMode mode);
    void setPolygonMode(PolygonMode mode);
    void setViewport(glm::ivec4 viewport);
};

} // namespace graphics

} // namespace reone
