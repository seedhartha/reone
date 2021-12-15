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

#pragma once

#include "framebuffer.h"
#include "options.h"
#include "renderbuffer.h"
#include "shaderprogram.h"
#include "texture.h"
#include "types.h"
#include "uniformbuffer.h"

namespace reone {

namespace graphics {

class Context : boost::noncopyable {
public:
    Context(GraphicsOptions options) :
        _options(std::move(options)) {
    }

    ~Context() { deinit(); }

    void init();
    void deinit();

    void clear(int mask);

    void useShaderProgram(std::shared_ptr<ShaderProgram> program);

    bool isDepthTestEnabled() const { return _depthTest; }

    const glm::ivec4 &viewport() const { return _viewport; }
    PolygonMode polygonMode() const { return _polygonMode; }
    BlendMode blendMode() const { return _blendMode; }

    void setViewport(glm::ivec4 viewport);
    void setDepthTestEnabled(bool enabled);
    void setBackFaceCullingEnabled(bool enabled);
    void setPolygonMode(PolygonMode mode);
    void setBlendMode(BlendMode mode);

    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);

    // Bindings

    void bindReadFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void bindDrawFramebuffer(std::shared_ptr<Framebuffer> framebuffer);

    void bindRenderbuffer(std::shared_ptr<Renderbuffer> renderbuffer);
    void bindTexture(int unit, std::shared_ptr<Texture> texture);
    void bindUniformBuffer(int index, std::shared_ptr<UniformBuffer> buffer);

    void unbindReadFramebuffer();
    void unbindDrawFramebuffer();

    void unbindRenderbuffer();
    void unbindTexture(int unit);
    void unbindUniformBuffer(int index);

    // END Bindings

private:
    GraphicsOptions _options;

    bool _inited {false};

    glm::ivec4 _viewport {0};
    bool _depthTest {false};
    bool _backFaceCulling {false};
    PolygonMode _polygonMode {PolygonMode::Fill};
    BlendMode _blendMode {BlendMode::None};

    int _textureUnit {0};
    int _uniformBufferBindingPoint {0};
    std::shared_ptr<ShaderProgram> _shaderProgram;

    // Bindings

    std::shared_ptr<Framebuffer> _boundFramebufferRead;
    std::shared_ptr<Framebuffer> _boundFramebufferDraw;

    std::shared_ptr<Renderbuffer> _boundRenderbuffer;
    std::vector<std::shared_ptr<Texture>> _boundTextures;
    std::vector<std::shared_ptr<UniformBuffer>> _boundUniformBuffers;

    // END Bindings

    void setActiveTextureUnit(int unit);
};

} // namespace graphics

} // namespace reone
