#pragma once

#include <memory>

#include "../texture.h"

#include "mesh.h"

namespace reone {

namespace resources {

class MdlFile;

}

namespace render {

/**
 * Textured mesh, part of a 3D model.
 *
 * @see reone::render::ModelNode
 * @see reone::render::Texture
 */
class ModelMesh : public Mesh {
public:
    ModelMesh(bool render);

    void initGL();
    void render(const std::shared_ptr<Texture> &diffuseOverride = nullptr) const;

    bool shouldRender() const;
    bool isTransparent() const;
    bool hasDiffuseTexture() const;
    bool hasEnvmapTexture() const;
    bool hasLightmapTexture() const;
    bool hasBumpyShinyTexture() const;

    const std::shared_ptr<Texture> &diffuseTexture() const;

private:
    bool _render { false };
    std::shared_ptr<Texture> _diffuse;
    std::shared_ptr<Texture> _envmap;
    std::shared_ptr<Texture> _lightmap;
    std::shared_ptr<Texture> _bumpyShiny;

    friend class resources::MdlFile;
};

} // namespace resources

} // namespace reone
