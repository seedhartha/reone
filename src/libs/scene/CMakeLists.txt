# Copyright (c) 2020-2023 The reone project contributors

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

set(SCENE_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/scene)
set(SCENE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/scene)

set(SCENE_HEADERS
    ${SCENE_INCLUDE_DIR}/animeventlistener.h
    ${SCENE_INCLUDE_DIR}/animproperties.h
    ${SCENE_INCLUDE_DIR}/collision.h
    ${SCENE_INCLUDE_DIR}/di/module.h
    ${SCENE_INCLUDE_DIR}/di/services.h
    ${SCENE_INCLUDE_DIR}/fogproperties.h
    ${SCENE_INCLUDE_DIR}/graph.h
    ${SCENE_INCLUDE_DIR}/graphs.h
    ${SCENE_INCLUDE_DIR}/grassproperties.h
    ${SCENE_INCLUDE_DIR}/node.h
    ${SCENE_INCLUDE_DIR}/node/camera.h
    ${SCENE_INCLUDE_DIR}/node/dummy.h
    ${SCENE_INCLUDE_DIR}/node/emitter.h
    ${SCENE_INCLUDE_DIR}/node/grass.h
    ${SCENE_INCLUDE_DIR}/node/grasscluster.h
    ${SCENE_INCLUDE_DIR}/node/light.h
    ${SCENE_INCLUDE_DIR}/node/mesh.h
    ${SCENE_INCLUDE_DIR}/node/model.h
    ${SCENE_INCLUDE_DIR}/node/modelnode.h
    ${SCENE_INCLUDE_DIR}/node/particle.h
    ${SCENE_INCLUDE_DIR}/node/sound.h
    ${SCENE_INCLUDE_DIR}/node/trigger.h
    ${SCENE_INCLUDE_DIR}/node/walkmesh.h
    ${SCENE_INCLUDE_DIR}/renderpass.h
    ${SCENE_INCLUDE_DIR}/renderpipeline.h
    ${SCENE_INCLUDE_DIR}/types.h
    ${SCENE_INCLUDE_DIR}/user.h)

set(SCENE_SOURCES
    ${SCENE_SOURCE_DIR}/di/module.cpp
    ${SCENE_SOURCE_DIR}/graph.cpp
    ${SCENE_SOURCE_DIR}/graphs.cpp
    ${SCENE_SOURCE_DIR}/node.cpp
    ${SCENE_SOURCE_DIR}/node/camera.cpp
    ${SCENE_SOURCE_DIR}/node/emitter.cpp
    ${SCENE_SOURCE_DIR}/node/grass.cpp
    ${SCENE_SOURCE_DIR}/node/light.cpp
    ${SCENE_SOURCE_DIR}/node/mesh.cpp
    ${SCENE_SOURCE_DIR}/node/model.cpp
    ${SCENE_SOURCE_DIR}/node/modelnode.cpp
    ${SCENE_SOURCE_DIR}/node/particle.cpp
    ${SCENE_SOURCE_DIR}/node/sound.cpp
    ${SCENE_SOURCE_DIR}/node/trigger.cpp
    ${SCENE_SOURCE_DIR}/node/walkmesh.cpp
    ${SCENE_SOURCE_DIR}/renderpass.cpp
    ${SCENE_SOURCE_DIR}/renderpipeline.cpp)

add_library(scene STATIC ${SCENE_HEADERS} ${SCENE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(scene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(scene PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(scene PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
