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

class Framebuffer;
class ShaderProgram;
class Texture;
class UniformBuffer;

enum class FramebufferBlitFilter {
    Nearest,
    Linear
};

struct FramebufferBlitFlags {
    static constexpr int color = 1;
    static constexpr int depth = 2;
    static constexpr int stencil = 3;

    static constexpr int colorDepth = color | depth;
};

class IContext {
public:
    virtual ~IContext() = default;

    virtual void clearColor(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;
    virtual void clearDepth() = 0;
    virtual void clearColorDepth(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;

    virtual void resetProgram() = 0;
    virtual void useProgram(ShaderProgram &program) = 0;

    virtual void resetDrawFramebuffer() = 0;
    virtual void resetReadFramebuffer() = 0;
    virtual void bindDrawFramebuffer(Framebuffer &buffer, std::vector<int> colorIndices = std::vector<int>()) = 0;
    virtual void bindReadFramebuffer(Framebuffer &buffer, std::optional<int> colorIdx = std::nullopt) = 0;
    virtual void blitFramebuffer(Framebuffer &source,
                                 Framebuffer &destination,
                                 const glm::ivec4 &srcRect,
                                 const glm::ivec4 &dstRect,
                                 int srcColorIdx = 0,
                                 int dstColorIdx = 0,
                                 int mask = FramebufferBlitFlags::color,
                                 FramebufferBlitFilter filter = FramebufferBlitFilter::Nearest) = 0;

    virtual void bindUniformBuffer(UniformBuffer &buffer, int index) = 0;
    virtual void bindTexture(Texture &texture, int unit = TextureUnits::mainTex) = 0;

    virtual FaceCullMode faceCulling() const = 0;
    virtual BlendMode blending() const = 0;
    virtual PolygonMode polygonMode() const = 0;

    virtual void pushFaceCulling(FaceCullMode mode) = 0;
    virtual void pushBlending(BlendMode mode) = 0;
    virtual void pushViewport(glm::ivec4 viewport) = 0;
    virtual void pushPolygonMode(PolygonMode mode) = 0;

    virtual void popFaceCulling() = 0;
    virtual void popBlending() = 0;
    virtual void popViewport() = 0;
    virtual void popPolygonMode() = 0;

    virtual void withDepthTest(DepthTestMode mode, const std::function<void()> &block) = 0;
    virtual void withFaceCulling(FaceCullMode mode, const std::function<void()> &block) = 0;
    virtual void withBlending(BlendMode mode, const std::function<void()> &block) = 0;
    virtual void withPolygonMode(PolygonMode mode, const std::function<void()> &block) = 0;
    virtual void withViewport(glm::ivec4 viewport, const std::function<void()> &block) = 0;
    virtual void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block) = 0;
};

class Context : public IContext, boost::noncopyable {
public:
    Context(GraphicsOptions &options) :
        _options(options) {
    }

    void init();

    void clearColor(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) override;
    void clearDepth() override;
    void clearColorDepth(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) override;

    void resetProgram() override;
    void useProgram(ShaderProgram &program) override;

    void resetDrawFramebuffer() override;
    void resetReadFramebuffer() override;
    void bindDrawFramebuffer(Framebuffer &buffer, std::vector<int> colorIndices = std::vector<int>()) override;
    void bindReadFramebuffer(Framebuffer &buffer, std::optional<int> colorIdx = std::nullopt) override;
    void blitFramebuffer(Framebuffer &source,
                         Framebuffer &destination,
                         const glm::ivec4 &srcRect,
                         const glm::ivec4 &dstRect,
                         int srcColorIdx = 0,
                         int dstColorIdx = 0,
                         int mask = FramebufferBlitFlags::color,
                         FramebufferBlitFilter filter = FramebufferBlitFilter::Nearest) override;

    void bindUniformBuffer(UniformBuffer &buffer, int index) override;
    void bindTexture(Texture &texture, int unit = TextureUnits::mainTex) override;

    FaceCullMode faceCulling() const override {
        return _faceCullModes.top();
    }

    BlendMode blending() const override {
        return _blendModes.top();
    }

    PolygonMode polygonMode() const override {
        return _polygonModes.top();
    }

    void pushFaceCulling(FaceCullMode mode) override;
    void pushBlending(BlendMode mode) override;
    void pushViewport(glm::ivec4 viewport) override;
    void pushPolygonMode(PolygonMode mode) override;

    void popFaceCulling() override;
    void popBlending() override;
    void popViewport() override;
    void popPolygonMode() override;

    void withDepthTest(DepthTestMode mode, const std::function<void()> &block) override;
    void withFaceCulling(FaceCullMode mode, const std::function<void()> &block) override;
    void withBlending(BlendMode mode, const std::function<void()> &block) override;
    void withPolygonMode(PolygonMode mode, const std::function<void()> &block) override;
    void withViewport(glm::ivec4 viewport, const std::function<void()> &block) override;
    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block) override;

private:
    GraphicsOptions &_options;

    bool _inited {false};

    std::optional<std::reference_wrapper<ShaderProgram>> _program;
    std::optional<std::reference_wrapper<Framebuffer>> _readFramebuffer;
    std::optional<std::reference_wrapper<Framebuffer>> _drawFramebuffer;
    int _activeTexUnit {0};

    // States

    std::stack<DepthTestMode> _depthTestModes;
    std::stack<FaceCullMode> _faceCullModes;
    std::stack<BlendMode> _blendModes;
    std::stack<PolygonMode> _polygonModes;
    std::stack<glm::ivec4> _viewports;

    // END States

    void setDepthTestMode(DepthTestMode mode);
    void setFaceCullMode(FaceCullMode mode);
    void setBlendMode(BlendMode mode);
    void setPolygonMode(PolygonMode mode);
    void setViewport(glm::ivec4 viewport);
};

} // namespace graphics

} // namespace reone
