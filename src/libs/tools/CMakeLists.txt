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

find_package(Boost COMPONENTS regex REQUIRED)

set(TOOLS_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/tools)
set(TOOLS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/tools)

set(TOOLS_HEADERS
    ${TOOLS_INCLUDE_DIR}/legacy/audio.h
    ${TOOLS_INCLUDE_DIR}/legacy/erf.h
    ${TOOLS_INCLUDE_DIR}/legacy/keybif.h
    ${TOOLS_INCLUDE_DIR}/legacy/ncs.h
    ${TOOLS_INCLUDE_DIR}/legacy/rim.h
    ${TOOLS_INCLUDE_DIR}/legacy/tool.h
    ${TOOLS_INCLUDE_DIR}/legacy/tpc.h
    ${TOOLS_INCLUDE_DIR}/lip/audioanalyzer.h
    ${TOOLS_INCLUDE_DIR}/lip/composer.h
    ${TOOLS_INCLUDE_DIR}/lip/shapeutil.h
    ${TOOLS_INCLUDE_DIR}/script/exprtree.h
    ${TOOLS_INCLUDE_DIR}/script/exprtreeoptimizer.h
    ${TOOLS_INCLUDE_DIR}/script/format/nsswriter.h
    ${TOOLS_INCLUDE_DIR}/script/format/pcodereader.h
    ${TOOLS_INCLUDE_DIR}/script/format/pcodewriter.h
    ${TOOLS_INCLUDE_DIR}/types.h)

set(TOOLS_SOURCES
    ${TOOLS_SOURCE_DIR}/legacy/audio.cpp
    ${TOOLS_SOURCE_DIR}/legacy/erf.cpp
    ${TOOLS_SOURCE_DIR}/legacy/keybif.cpp
    ${TOOLS_SOURCE_DIR}/legacy/ncs.cpp
    ${TOOLS_SOURCE_DIR}/legacy/rim.cpp
    ${TOOLS_SOURCE_DIR}/legacy/tpc.cpp
    ${TOOLS_SOURCE_DIR}/lip/audioanalyzer.cpp
    ${TOOLS_SOURCE_DIR}/lip/composer.cpp
    ${TOOLS_SOURCE_DIR}/script/exprtree.cpp
    ${TOOLS_SOURCE_DIR}/script/exprtreeoptimizer.cpp
    ${TOOLS_SOURCE_DIR}/script/format/nsswriter.cpp
    ${TOOLS_SOURCE_DIR}/script/format/pcodereader.cpp
    ${TOOLS_SOURCE_DIR}/script/format/pcodewriter.cpp)

add_library(tools ${TOOLS_HEADERS} ${TOOLS_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(tools PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(tools PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(tools PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(tools PRIVATE game)
