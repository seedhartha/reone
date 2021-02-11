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

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "framebuffer.h"
#include "texture.h"

namespace reone {

namespace render {

class IrradianceMaps {
public:
    static IrradianceMaps &instance();

    ~IrradianceMaps();

    void init();
    void deinit();

    /**
     * Computes all queued irradiance maps.
     */
    void refresh();

    std::shared_ptr<Texture> get(const Texture *envmap);

private:
    Framebuffer _framebuffer;

    bool _inited { false };
    std::unordered_map<const Texture *, std::shared_ptr<Texture>> _irradianceByEnvmap;
    std::set<const Texture *> _computeQueue;

    IrradianceMaps();

    std::shared_ptr<Texture> computeIrradianceMap(const Texture *envmap);
};

} // namespace render

} // namespace reone
