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

#include "types.h"

namespace reone {

namespace graphics {

struct GraphicsOptions {
    int width {1024};
    int height {768};
    bool fullscreen {false};
    bool vsync {true};
    bool fxaa {true};
    bool sharpen {true};
    bool grass {true};
    bool ssr {true};
    bool ssao {true};
    TextureQuality textureQuality {TextureQuality::High};
    int shadowResolution {2048};
    float drawDistance {kDefaultObjectDrawDistance};
};

} // namespace graphics

} // namespace reone
