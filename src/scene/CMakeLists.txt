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

set(SCENE_HEADERS
    animeventlistener.h
    animproperties.h
    collision.h
    graph.h
    graphs.h
    node.h
    node/camera.h
    node/dummy.h
    node/emitter.h
    node/grass.h
    node/grasscluster.h
    node/light.h
    node/mesh.h
    node/model.h
    node/modelnode.h
    node/particle.h
    node/sound.h
    node/trigger.h
    node/walkmesh.h
    types.h
    user.h)

set(SCENE_SOURCES
    graph.cpp
    graphs.cpp
    node.cpp
    node/camera.cpp
    node/emitter.cpp
    node/grass.cpp
    node/light.cpp
    node/mesh.cpp
    node/model.cpp
    node/modelnode.cpp
    node/particle.cpp
    node/sound.cpp
    node/trigger.cpp
    node/walkmesh.cpp)

add_library(scene STATIC ${SCENE_HEADERS} ${SCENE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(scene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(scene PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
