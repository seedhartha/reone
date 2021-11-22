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
    graph.h
    graphs.h
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
    node/scenenode.h
    node/walkmesh.h
    pipeline/control.h
    pipeline/world.h
    types.h)

set(SCENE_SOURCES
    graph.cpp
    graphs.cpp
    node/camera.cpp
    node/emitter.cpp
    node/grass.cpp
    node/light.cpp
    node/mesh.cpp
    node/model.cpp
    node/modelnode.cpp
    node/particle.cpp
    node/scenenode.cpp
    pipeline/control.cpp
    pipeline/world.cpp)

add_library(libscene STATIC ${SCENE_HEADERS} ${SCENE_SOURCES})
set_target_properties(libscene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libscene PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)
