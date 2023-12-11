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

set(REONE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/reone)

set(REONE_HEADERS
    ${REONE_SOURCE_DIR}/engine.h
    ${REONE_SOURCE_DIR}/options.h
    ${REONE_SOURCE_DIR}/optionsparser.h)

set(REONE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/apps/highperfgfx.cpp
    ${REONE_SOURCE_DIR}/engine.cpp
    ${REONE_SOURCE_DIR}/main.cpp
    ${REONE_SOURCE_DIR}/optionsparser.cpp)

if(WIN32)
    list(APPEND REONE_SOURCES ${CMAKE_SOURCE_DIR}/assets/reone.rc)
endif()

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(reone PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone PRIVATE game ${Boost_PROGRAM_OPTIONS_LIBRARY})

list(APPEND InstallTargets reone)
