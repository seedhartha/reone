#pragma once

#include "../render/texture.h"

#include "binfile.h"

namespace reone {

namespace resources {

class TpcFile : public BinaryFile {
public:
    TpcFile(const std::string &resRef, render::TextureType type);
    std::shared_ptr<render::Texture> texture() const;

private:
    enum class EncodingType {
        Grayscale = 1,
        RGB = 2,
        RGBA = 4
    };

    std::string _resRef;
    render::TextureType _type;
    uint32_t _dataSize { 0 };
    bool _compressed { false };
    uint16_t _width { 0 };
    uint16_t _height { 0 };
    EncodingType _encoding { EncodingType::Grayscale };
    uint8_t _mipMapCount { 0 };
    bool _cubeMap { false };
    std::shared_ptr<render::Texture> _texture;

    void doLoad() override;
    int getMipMapSize(int idx);
    void loadTexture();
    render::PixelFormat pixelFormat() const;
};

} // namespace resources

} // namespace reone
