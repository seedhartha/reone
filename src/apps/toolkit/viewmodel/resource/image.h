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

#include "reone/resource/id.h"
#include "reone/system/stream/input.h"

#include "../../property.h"
#include "../resource.h"

namespace reone {

struct ImageContent {
    std::shared_ptr<ByteBuffer> tgaBytes;
    std::shared_ptr<ByteBuffer> txiBytes;

    ImageContent() {
    }

    ImageContent(std::shared_ptr<ByteBuffer> tgaBytes, std::shared_ptr<ByteBuffer> txiBytes) :
        tgaBytes(tgaBytes),
        txiBytes(txiBytes) {
    }
};

class ImageResourceViewModel : public ResourceViewModel {
public:
    void openImage(const resource::ResourceId &id, IInputStream &stream);

    Property<ImageContent> &imageContent() { return _imageContent; }

private:
    Property<ImageContent> _imageContent;
};

} // namespace reone
