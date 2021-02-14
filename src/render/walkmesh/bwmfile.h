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

#include "../../resource/format/binfile.h"

#include "walkmesh.h"

namespace reone {

namespace render {

class BwmFile : public resource::BinaryFile {
public:
    BwmFile();

    std::shared_ptr<Walkmesh> walkmesh() const { return _walkmesh; }

private:
    uint32_t _type { 0 };
    uint32_t _vertexCount { 0 };
    uint32_t _vertexOffset { 0 };
    uint32_t _faceCount { 0 };
    uint32_t _faceOffset { 0 };
    uint32_t _faceTypeOffset { 0 };
    std::vector<float> _vertices;
    std::vector<uint32_t> _indices;
    std::vector<uint32_t> _faceTypes;
    std::shared_ptr<Walkmesh> _walkmesh;

    void doLoad() override;

    void loadVertices();
    void loadFaces();
    void loadFaceTypes();
    void makeWalkmesh();
};

} // namespace render

} // namespace reone
