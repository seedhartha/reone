# Copyright (c) 2020-2021 The reone project contributors

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set(GRAPHICS_HEADERS
    aabb.h
    barycentricutil.h
    beziercurve.h
    context.h
    cursor.h
    eventhandler.h
    features.h
    font.h
    fonts.h
    framebuffer.h
    lip/animation.h
    lip/format/lipreader.h
    lip/format/lipwriter.h
    lip/lips.h
    material.h
    materials.h
    mesh/mesh.h
    mesh/meshes.h
    mesh/vertexattributes.h
    model/animatedproperty.h
    model/animation.h
    model/format/mdlreader.h
    model/model.h
    model/modelnode.h
    model/models.h
    options.h
    pbribl.h
    pixelutil.h
    renderbuffer.h
    shader/shaders.h
    texture/dxtutil.h
    texture/format/curreader.h
    texture/format/tgareader.h
    texture/format/tgawriter.h
    texture/format/tpcreader.h
    texture/format/txireader.h
    texture/texture.h
    texture/textures.h
    texture/textureutil.h
    textutil.h
    triangleutil.h
    types.h
    walkmesh/format/bwmreader.h
    walkmesh/walkmesh.h
    walkmesh/walkmeshes.h
    window.h)

set(GRAPHICS_SOURCES
    aabb.cpp
    context.cpp
    cursor.cpp
    features.cpp
    font.cpp
    fonts.cpp
    framebuffer.cpp
    lip/animation.cpp
    lip/format/lipreader.cpp
    lip/format/lipwriter.cpp
    lip/lips.cpp
    materials.cpp
    mesh/mesh.cpp
    mesh/meshes.cpp
    model/animation.cpp
    model/format/mdlreader.cpp
    model/model.cpp
    model/modelnode.cpp
    model/models.cpp
    pbribl.cpp
    pixelutil.cpp
    renderbuffer.cpp
    shader/shaders.cpp
    shader/shaders_common.cpp
    shader/shaders_pbr.cpp
    shader/shaders_phong.cpp
    texture/dxtutil.cpp
    texture/format/curreader.cpp
    texture/format/tgareader.cpp
    texture/format/tgawriter.cpp
    texture/format/tpcreader.cpp
    texture/format/txireader.cpp
    texture/texture.cpp
    texture/textures.cpp
    texture/textureutil.cpp
    textutil.cpp
    walkmesh/format/bwmreader.cpp
    walkmesh/walkmesh.cpp
    walkmesh/walkmeshes.cpp
    window.cpp)

add_library(graphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES})
set_target_properties(graphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(graphics PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)
target_link_libraries(graphics PUBLIC GLEW::GLEW ${OPENGL_LIBRARIES})

if(MSVC)
    target_link_libraries(graphics PUBLIC SDL2::SDL2)
else()
    target_link_libraries(graphics PUBLIC ${SDL2_LIBRARIES})
endif()
