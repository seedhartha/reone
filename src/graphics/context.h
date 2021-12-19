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

class GraphicsContext : boost::noncopyable {
public:
    GraphicsContext(GraphicsOptions options) :
        _options(std::move(options)) {
    }

    ~GraphicsContext() { deinit(); }

    void init();
    void deinit();

    void useShaderProgram(std::shared_ptr<ShaderProgram> program);

    void bindTexture(int unit, std::shared_ptr<Texture> texture);
    void unbindTexture(int unit);

    BlendMode blendMode() const { return _blendMode; }

    void setBackFaceCullingEnabled(bool enabled);
    void setBlendMode(BlendMode mode);

    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);

private:
    GraphicsOptions _options;

    bool _inited {false};

    bool _backFaceCulling {false};
    BlendMode _blendMode {BlendMode::None};
    int _textureUnit {0};

    std::shared_ptr<ShaderProgram> _shaderProgram;
    std::vector<std::shared_ptr<Texture>> _textures;

    void setActiveTextureUnit(int unit);
};

} // namespace graphics

} // namespace reone
