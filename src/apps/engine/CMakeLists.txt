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

set(ENGINE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/engine)

set(REONE_HEADERS
    ${ENGINE_SOURCE_DIR}/di/module/audio.h
    ${ENGINE_SOURCE_DIR}/di/module/game.h
    ${ENGINE_SOURCE_DIR}/di/module/graphics.h
    ${ENGINE_SOURCE_DIR}/di/module/resource.h
    ${ENGINE_SOURCE_DIR}/di/module/scene.h
    ${ENGINE_SOURCE_DIR}/di/module/script.h
    ${ENGINE_SOURCE_DIR}/di/module/system.h
    ${ENGINE_SOURCE_DIR}/di/services.h
    ${ENGINE_SOURCE_DIR}/engine.h
    ${ENGINE_SOURCE_DIR}/gameprobe.h
    ${ENGINE_SOURCE_DIR}/options.h
    ${ENGINE_SOURCE_DIR}/optionsparser.h)

set(REONE_SOURCES
    ${ENGINE_SOURCE_DIR}/di/module/audio.cpp
    ${ENGINE_SOURCE_DIR}/di/module/game.cpp
    ${ENGINE_SOURCE_DIR}/di/module/graphics.cpp
    ${ENGINE_SOURCE_DIR}/di/module/resource.cpp
    ${ENGINE_SOURCE_DIR}/di/module/scene.cpp
    ${ENGINE_SOURCE_DIR}/di/module/script.cpp
    ${ENGINE_SOURCE_DIR}/di/module/system.cpp
    ${ENGINE_SOURCE_DIR}/di/services.cpp
    ${ENGINE_SOURCE_DIR}/engine.cpp
    ${ENGINE_SOURCE_DIR}/gameprobe.cpp
    ${ENGINE_SOURCE_DIR}/highperfgfx.cpp
    ${ENGINE_SOURCE_DIR}/main.cpp
    ${ENGINE_SOURCE_DIR}/optionsparser.cpp)

if(WIN32)
    list(APPEND REONE_SOURCES ${CMAKE_SOURCE_DIR}/assets/reone.rc)
endif()

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(reone PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone PRIVATE game ${Boost_PROGRAM_OPTIONS_LIBRARY})

list(APPEND InstallTargets reone)
