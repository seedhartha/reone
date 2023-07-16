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

set(LAUNCHER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/launcher)

set(LAUNCHER_HEADERS
    ${LAUNCHER_SOURCE_DIR}/app.h
    ${LAUNCHER_SOURCE_DIR}/frame.h)

set(LAUNCHER_SOURCES
    ${LAUNCHER_SOURCE_DIR}/app.cpp
    ${LAUNCHER_SOURCE_DIR}/frame.cpp)

if(WIN32)
    list(APPEND LAUNCHER_SOURCES ${CMAKE_SOURCE_DIR}/assets/reone.rc)
    add_executable(launcher WIN32 ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES} ${CLANG_FORMAT_PATH})
else()
    add_executable(launcher ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES})
endif()

set_target_properties(launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(launcher PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(launcher PRIVATE system ${wxWidgets_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

list(APPEND InstallTargets launcher)
