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

namespace reone {

namespace graphics {

class Context;
class Meshes;
class Shaders;
class Texture;

/**
 * Computes and caches PBR IBL textures, i.e. irradiance maps, prefiltered
 * environment maps and BRDF lookup textures.
 */
class PBRIBL : boost::noncopyable {
public:
    struct Derived {
        std::shared_ptr<Texture> irradianceMap;
        std::shared_ptr<Texture> prefilterMap;
        std::shared_ptr<Texture> brdfLookup;
    };

    PBRIBL(Context &context, Meshes &meshes, Shaders &shaders);
    ~PBRIBL();

    void init();
    void deinit();

    /**
     * Computes derived textures for all queued environment maps.
     */
    void refresh();

    bool contains(const Texture *envmap);

    /**
     * Retrieves derived textures for the specified environment map.
     *
     * @return true if derived textures were found in the cache, false otherwise
     */
    bool getDerived(const Texture *envmap, Derived &derived);

private:
    Context &_context;
    Meshes &_meshes;
    Shaders &_shaders;

    bool _inited { false };
    std::set<const Texture *> _envmapQueue;
    std::unordered_map<const Texture *, Derived> _derivedByEnvmap;

    // Framebuffers

    Framebuffer _irradianceFB;
    Framebuffer _prefilterFB;
    Framebuffer _brdfLookupFB;

    // END Framebuffers

    std::shared_ptr<Texture> computeIrradianceMap(const Texture *envmap);
    std::shared_ptr<Texture> computePrefilterMap(const Texture *envmap);
    std::shared_ptr<Texture> computeBRDFLookup(const Texture *envmap);
};

} // namespace graphics

} // namespace reone
