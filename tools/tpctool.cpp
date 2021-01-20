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

#include "tools.h"

#include <stdexcept>

#include "s3tc.h"

#include "../src/common/log.h"
#include "../src/render/image/tpcfile.h"

extern "C" {

#include "tga.h"

}

using namespace std;

using namespace reone::render;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

static void convertTpcToTga(const Texture &texture, TGAHeader &header, TGAData &data) {
    int layerCount = static_cast<int>(texture.layers().size());
    if (layerCount == 0) {
        throw invalid_argument("TPC: convert: image is empty");
    }
    int w = texture.width();
    int h = texture.height();
    bool alpha = texture.pixelFormat() == PixelFormat::RGBA || texture.pixelFormat() == PixelFormat::DXT5;

    int totalHeight = layerCount * w;
    int pixelsSize = (alpha ? 4ll : 3ll) * w * totalHeight;
    auto pixels = make_unique<uint8_t[]>(pixelsSize);
    uint8_t *pixelsPtr = &pixels[0];

    for (int i = 0; i < layerCount; ++i) {
        const Texture::MipMap &mipMap = texture.layers()[i].mipMaps.front();
        const uint8_t *mipMapPtr = reinterpret_cast<const uint8_t *>(&mipMap.data[0]);

        switch (texture.pixelFormat()) {
            case PixelFormat::Grayscale:
                for (int i = 0; i < w * h; ++i) {
                    uint8_t val = *(mipMapPtr++);
                    *(pixelsPtr++) = val;
                    *(pixelsPtr++) = val;
                    *(pixelsPtr++) = val;
                }
                break;
            case PixelFormat::RGB:
                for (int i = 0; i < w * h; ++i) {
                    *(pixelsPtr++) = *(mipMapPtr++);
                    *(pixelsPtr++) = *(mipMapPtr++);
                    *(pixelsPtr++) = *(mipMapPtr++);
                }
                break;
            case PixelFormat::RGBA:
                for (int i = 0; i < w * h; ++i) {
                    *(pixelsPtr++) = *(mipMapPtr++);
                    *(pixelsPtr++) = *(mipMapPtr++);
                    *(pixelsPtr++) = *(mipMapPtr++);
                    *(pixelsPtr++) = *(mipMapPtr++);
                }
                break;
            case PixelFormat::DXT1: {
                vector<unsigned long> pixelValues(static_cast<size_t>(w) * h);
                BlockDecompressImageDXT1(mipMap.width, mipMap.height, mipMapPtr, &pixelValues[0]);
                unsigned long *pixelValuesPtr = &pixelValues[0];
                for (int i = 0; i < w * h; ++i) {
                    unsigned long rgba = *(pixelValuesPtr++);
                    *(pixelsPtr++) = (rgba >> 24) & 0xff;
                    *(pixelsPtr++) = (rgba >> 16) & 0xff;
                    *(pixelsPtr++) = (rgba >> 8) & 0xff;
                }
                break;
            }
            case PixelFormat::DXT5: {
                vector<unsigned long> pixelValues(static_cast<size_t>(w) * h);
                BlockDecompressImageDXT5(mipMap.width, mipMap.height, mipMapPtr, &pixelValues[0]);
                unsigned long *pixelValuesPtr = &pixelValues[0];
                for (int i = 0; i < w * h; ++i) {
                    unsigned long rgba = *(pixelValuesPtr++);
                    *(pixelsPtr++) = (rgba >> 24) & 0xff;
                    *(pixelsPtr++) = (rgba >> 16) & 0xff;
                    *(pixelsPtr++) = (rgba >> 8) & 0xff;
                    *(pixelsPtr++) = rgba & 0xff;
                }
                break;
            }
            default:
                throw invalid_argument("Unsupported pixel format: " + to_string(static_cast<int>(texture.pixelFormat())));
        }
    }

    memset(&header, 0, sizeof(TGAHeader));
    header.img_t = 2;
    header.width = w;
    header.height = totalHeight;
    header.depth = alpha ? 32 : 24;
    header.alpha = alpha ? 8 : 0;

    memset(&data, 0, sizeof(TGAData));
    data.img_data = &pixels[0];
    data.flags = TGA_IMAGE_DATA | TGA_RGB;
}

void TpcTool::convert(const fs::path &path, const fs::path &destPath) const {
    // Read TPC

    TpcFile tpc("", TextureType::GUI, true);
    tpc.load(path);

    // Convert TPC to TGA

    TGAHeader header;
    TGAData data;
    convertTpcToTga(*tpc.texture(), header, data);

    // Write TGA

    fs::path tgaPath(destPath);
    tgaPath.append(path.filename().string());
    tgaPath.replace_extension("tga");

    TGA *tga = TGAOpen(const_cast<char *>(tgaPath.string().c_str()), "wb");
    if (!tga) {
        error("TPC: write failed: " + tgaPath.string());
        return;
    }

    tga->hdr = header;
    TGAWriteImage(tga, &data);

    if (tga->last != TGA_OK) {
        string msg(TGAStrError(tga->last));
        error("TPC: convert failed: " + msg);
    }

    // Cleanup

    TGAClose(tga);
}

} // namespace tools

} // namespace reone
