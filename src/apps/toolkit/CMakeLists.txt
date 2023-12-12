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

set(TOOLKIT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/toolkit)

set(TOOLKIT_HEADERS
    ${TOOLKIT_SOURCE_DIR}/app.h
    ${TOOLKIT_SOURCE_DIR}/ui/composelipdialog.h
    ${TOOLKIT_SOURCE_DIR}/ui/mainframe.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/eventhandler.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/main.h)

set(TOOLKIT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/apps/highperfgfx.cpp
    ${TOOLKIT_SOURCE_DIR}/app.cpp
    ${TOOLKIT_SOURCE_DIR}/ui/composelipdialog.cpp
    ${TOOLKIT_SOURCE_DIR}/ui/mainframe.cpp
    ${TOOLKIT_SOURCE_DIR}/viewmodel/main.cpp)

if(WIN32)
    list(APPEND TOOLKIT_SOURCES ${CMAKE_SOURCE_DIR}/assets/toolkit.rc)
    add_executable(toolkit WIN32 ${TOOLKIT_HEADERS} ${TOOLKIT_SOURCES} ${CLANG_FORMAT_PATH})
else()
    add_executable(toolkit ${TOOLKIT_HEADERS} ${TOOLKIT_SOURCES} ${CLANG_FORMAT_PATH})
endif()

set_target_properties(toolkit PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(toolkit PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(toolkit PRIVATE tools ${wxWidgets_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
if(MSVC)
    target_link_libraries(toolkit PRIVATE SDL2::SDL2)
else()
    target_link_libraries(toolkit PRIVATE ${SDL2_LIBRARIES})
endif()

list(APPEND InstallTargets toolkit)

add_custom_target(copy_cmudict ALL ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/extern/cmudict/cmudict.dict ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin/cmudict.dict)
