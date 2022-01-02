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
#include "types.h"

namespace reone {

namespace graphics {

class GraphicsContext;
class IScene;
class Meshes;
class Shaders;
class Textures;

class Pipeline : boost::noncopyable {
public:
    Pipeline(
        GraphicsOptions options,
        GraphicsContext &graphicsContext,
        Meshes &meshes,
        Shaders &shaders,
        Textures &textures) :
        _options(std::move(options)),
        _graphicsContext(graphicsContext),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures) {
    }

    std::shared_ptr<Texture> draw(IScene &scene, const glm::ivec2 &dim);

private:
    struct Vec2Hasher {
        size_t operator()(const glm::ivec2 &dim) const {
            std::hash<int> intHash;
            return intHash(dim.x) ^ intHash(dim.y);
        }
    };

    struct Attachments {
        std::shared_ptr<Framebuffer> fbPing;
        std::shared_ptr<Framebuffer> fbPong;
        std::shared_ptr<Framebuffer> fbPointLightShadows;
        std::shared_ptr<Framebuffer> fbDirectionalLightShadows;
        std::shared_ptr<Framebuffer> fbGeometry;
        std::shared_ptr<Framebuffer> fbSSR;
        std::shared_ptr<Framebuffer> fbOutput;

        std::shared_ptr<Texture> cbPing;
        std::shared_ptr<Texture> cbPong;
        std::shared_ptr<Texture> cbGeometry1;
        std::shared_ptr<Texture> cbGeometry2;
        std::shared_ptr<Texture> cbGeometryEyeNormal;
        std::shared_ptr<Texture> cbGeometryRoughness;
        std::shared_ptr<Texture> cbSSR;
        std::shared_ptr<Texture> cbOutput;

        std::shared_ptr<Renderbuffer> dbCommon;
        std::shared_ptr<Texture> dbDirectionalLightShadows;
        std::shared_ptr<Texture> dbPointLightShadows;
        std::shared_ptr<Texture> dbGeometry;
        std::shared_ptr<Texture> dbOutput;
    };

    GraphicsOptions _options;

    glm::mat4 _shadowLightSpace[kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes[2] {glm::vec4(0.0f)};

    std::unordered_map<glm::ivec2, Attachments, Vec2Hasher> _attachments;

    // Services

    GraphicsContext &_graphicsContext;
    Meshes &_meshes;
    Shaders &_shaders;
    Textures &_textures;

    // END Services

    void initAttachments(glm::ivec2 extent);

    void computeLightSpaceMatrices(IScene &scene);

    void drawShadows(IScene &scene, Attachments &attachments);
    void drawGeometry(IScene &scene, Attachments &attachments, bool translucent = false);

    void drawSSR(IScene &scene, const glm::ivec2 &dim, Attachments &attachments);
    void applyBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool vertical = false);
    void drawComposite(Attachments &attachments, Framebuffer &dst);
    void applyFXAA(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst);

    void blitFramebuffer(const glm::ivec2 &dim, Framebuffer &src, int srcColorIdx, Framebuffer &dst, int dstColorIdx, bool depth = false);
};

} // namespace graphics

} // namespace reone
