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
    ${TOOLKIT_SOURCE_DIR}/binding/command.h
    ${TOOLKIT_SOURCE_DIR}/binding/property.h
    ${TOOLKIT_SOURCE_DIR}/view/composelipdialog.h
    ${TOOLKIT_SOURCE_DIR}/view/mainframe.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/audiopanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/gffpanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/imagepanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/modelpanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/ncspanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/nsspanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/tablepanel.h
    ${TOOLKIT_SOURCE_DIR}/view/resource/textpanel.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/audio.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/explorer.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/gff.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/image.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/model.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/ncs.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/nss.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/table.h
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/text.h)

set(TOOLKIT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/apps/highperfgfx.cpp
    ${TOOLKIT_SOURCE_DIR}/app.cpp
    ${TOOLKIT_SOURCE_DIR}/view/composelipdialog.cpp
    ${TOOLKIT_SOURCE_DIR}/view/mainframe.cpp
    ${TOOLKIT_SOURCE_DIR}/viewmodel/resource/explorer.cpp)

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
