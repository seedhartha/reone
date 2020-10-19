/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../render/texture.h"

#include "../binfile.h"

namespace reone {

namespace resource {

class CurFile : public BinaryFile {
public:
    CurFile(const std::string &resRef);

    std::shared_ptr<render::Texture> texture();

private:
    std::string _resRef;
    uint16_t _bitCount { 0 };
    int _width { 0 };
    int _height { 0 };
    std::shared_ptr<render::Texture> _texture;

    void doLoad() override;
    void loadHeader();
    void loadData();
};

} // namespace resources

} // namespace reone
