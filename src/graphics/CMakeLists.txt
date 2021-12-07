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
    animatedproperty.h
    animation.h
    barycentricutil.h
    beziercurve.h
    context.h
    cursor.h
    dxtutil.h
    eventhandler.h
    font.h
    fonts.h
    format/bwmreader.h
    format/curreader.h
    format/lipreader.h
    format/lipwriter.h
    format/mdlreader.h
    format/tgareader.h
    format/tgawriter.h
    format/tpcreader.h
    format/txireader.h
    framebuffer.h
    lipanimation.h
    lips.h
    mesh.h
    meshes.h
    model.h
    modelnode.h
    models.h
    options.h
    pixelutil.h
    renderbuffer.h
    shaders.h
    texture.h
    textures.h
    textureutil.h
    textutil.h
    triangleutil.h
    types.h
    walkmesh.h
    walkmeshes.h
    window.h)

set(GRAPHICS_SOURCES
    aabb.cpp
    animation.cpp
    context.cpp
    cursor.cpp
    dxtutil.cpp
    font.cpp
    fonts.cpp
    format/bwmreader.cpp
    format/curreader.cpp
    format/lipreader.cpp
    format/lipwriter.cpp
    format/mdlreader.cpp
    format/tgareader.cpp
    format/tgawriter.cpp
    format/tpcreader.cpp
    format/txireader.cpp
    framebuffer.cpp
    lipanimation.cpp
    lips.cpp
    mesh.cpp
    meshes.cpp
    model.cpp
    modelnode.cpp
    models.cpp
    pixelutil.cpp
    renderbuffer.cpp
    shaders.cpp
    texture.cpp
    textures.cpp
    textureutil.cpp
    textutil.cpp
    walkmesh.cpp
    walkmeshes.cpp
    window.cpp)

add_library(graphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES})
set_target_properties(graphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(graphics PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(graphics PUBLIC GLEW::GLEW ${OPENGL_LIBRARIES})

if(MSVC)
    target_link_libraries(graphics PUBLIC SDL2::SDL2)
else()
    target_link_libraries(graphics PUBLIC ${SDL2_LIBRARIES})
endif()
