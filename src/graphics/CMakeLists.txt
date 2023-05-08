# Copyright (c) 2020-2022 The reone project contributors

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

set(GRAPHICS_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/graphics)

set(GRAPHICS_HEADERS
    ${GRAPHICS_INCLUDE_DIR}/aabb.h
    ${GRAPHICS_INCLUDE_DIR}/animatedproperty.h
    ${GRAPHICS_INCLUDE_DIR}/animation.h
    ${GRAPHICS_INCLUDE_DIR}/attachment.h
    ${GRAPHICS_INCLUDE_DIR}/barycentricutil.h
    ${GRAPHICS_INCLUDE_DIR}/camera.h
    ${GRAPHICS_INCLUDE_DIR}/camera/orthographic.h
    ${GRAPHICS_INCLUDE_DIR}/camera/perspective.h
    ${GRAPHICS_INCLUDE_DIR}/context.h
    ${GRAPHICS_INCLUDE_DIR}/cursor.h
    ${GRAPHICS_INCLUDE_DIR}/dxtutil.h
    ${GRAPHICS_INCLUDE_DIR}/eventhandler.h
    ${GRAPHICS_INCLUDE_DIR}/font.h
    ${GRAPHICS_INCLUDE_DIR}/fonts.h
    ${GRAPHICS_INCLUDE_DIR}/format/bwmreader.h
    ${GRAPHICS_INCLUDE_DIR}/format/curreader.h
    ${GRAPHICS_INCLUDE_DIR}/format/lipreader.h
    ${GRAPHICS_INCLUDE_DIR}/format/lipwriter.h
    ${GRAPHICS_INCLUDE_DIR}/format/mdlreader.h
    ${GRAPHICS_INCLUDE_DIR}/format/tgareader.h
    ${GRAPHICS_INCLUDE_DIR}/format/tgawriter.h
    ${GRAPHICS_INCLUDE_DIR}/format/tpcreader.h
    ${GRAPHICS_INCLUDE_DIR}/format/txireader.h
    ${GRAPHICS_INCLUDE_DIR}/framebuffer.h
    ${GRAPHICS_INCLUDE_DIR}/glsl/common.h
    ${GRAPHICS_INCLUDE_DIR}/glsl/fragment.h
    ${GRAPHICS_INCLUDE_DIR}/glsl/geometry.h
    ${GRAPHICS_INCLUDE_DIR}/glsl/vertex.h
    ${GRAPHICS_INCLUDE_DIR}/lipanimation.h
    ${GRAPHICS_INCLUDE_DIR}/lipanimations.h
    ${GRAPHICS_INCLUDE_DIR}/lumautil.h
    ${GRAPHICS_INCLUDE_DIR}/mesh.h
    ${GRAPHICS_INCLUDE_DIR}/meshes.h
    ${GRAPHICS_INCLUDE_DIR}/model.h
    ${GRAPHICS_INCLUDE_DIR}/modelnode.h
    ${GRAPHICS_INCLUDE_DIR}/models.h
    ${GRAPHICS_INCLUDE_DIR}/options.h
    ${GRAPHICS_INCLUDE_DIR}/pipeline.h
    ${GRAPHICS_INCLUDE_DIR}/pixelutil.h
    ${GRAPHICS_INCLUDE_DIR}/renderbuffer.h
    ${GRAPHICS_INCLUDE_DIR}/scene.h
    ${GRAPHICS_INCLUDE_DIR}/services.h
    ${GRAPHICS_INCLUDE_DIR}/shader.h
    ${GRAPHICS_INCLUDE_DIR}/shaderprogram.h
    ${GRAPHICS_INCLUDE_DIR}/shaders.h
    ${GRAPHICS_INCLUDE_DIR}/texture.h
    ${GRAPHICS_INCLUDE_DIR}/textures.h
    ${GRAPHICS_INCLUDE_DIR}/textureutil.h
    ${GRAPHICS_INCLUDE_DIR}/textutil.h
    ${GRAPHICS_INCLUDE_DIR}/triangleutil.h
    ${GRAPHICS_INCLUDE_DIR}/types.h
    ${GRAPHICS_INCLUDE_DIR}/uniformbuffer.h
    ${GRAPHICS_INCLUDE_DIR}/uniforms.h
    ${GRAPHICS_INCLUDE_DIR}/walkmesh.h
    ${GRAPHICS_INCLUDE_DIR}/walkmeshes.h
    ${GRAPHICS_INCLUDE_DIR}/window.h)

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
    lipanimations.cpp
    mesh.cpp
    meshes.cpp
    model.cpp
    modelnode.cpp
    models.cpp
    pipeline.cpp
    pixelutil.cpp
    renderbuffer.cpp
    shader.cpp
    shaderprogram.cpp
    shaders.cpp
    texture.cpp
    textures.cpp
    textureutil.cpp
    textutil.cpp
    uniformbuffer.cpp
    uniforms.cpp
    walkmesh.cpp
    walkmeshes.cpp
    window.cpp)

add_library(graphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(graphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(graphics PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(graphics PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(graphics PUBLIC GLEW::GLEW ${OPENGL_LIBRARIES})

if(MSVC)
    target_link_libraries(graphics PUBLIC SDL2::SDL2)
else()
    target_link_libraries(graphics PUBLIC ${SDL2_LIBRARIES})
endif()
