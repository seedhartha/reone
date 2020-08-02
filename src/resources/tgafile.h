#pragma once

#include "binfile.h"

#include "../render/texture.h"

namespace reone {

namespace resources {

class TgaFile : public BinaryFile {
public:
    TgaFile(const std::string &resRef, render::TextureType type);
    std::shared_ptr<render::Texture> texture() const;

private:
    enum class ImageType {
        RGBA = 2,
        Grayscale = 3
    };

    std::string _resRef;
    render::TextureType _texType;
    ImageType _imageType;
    int _width { 0 };
    int _height { 0 };
    std::shared_ptr<render::Texture> _texture;

    void doLoad() override;
    void loadTexture();
};

} // namespace resources

} // namespace reone
