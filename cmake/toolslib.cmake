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

find_package(Boost COMPONENTS regex REQUIRED)

set(TOOLSLIB_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/tools)
set(TOOLSLIB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/tools)

set(TOOLSLIB_HEADERS
    ${TOOLSLIB_INCLUDE_DIR}/2da.h
    ${TOOLSLIB_INCLUDE_DIR}/audio.h
    ${TOOLSLIB_INCLUDE_DIR}/erf.h
    ${TOOLSLIB_INCLUDE_DIR}/gff.h
    ${TOOLSLIB_INCLUDE_DIR}/keybif.h
    ${TOOLSLIB_INCLUDE_DIR}/lip.h
    ${TOOLSLIB_INCLUDE_DIR}/ncs.h
    ${TOOLSLIB_INCLUDE_DIR}/rim.h
    ${TOOLSLIB_INCLUDE_DIR}/script/expressiontree.h
    ${TOOLSLIB_INCLUDE_DIR}/script/format/nsswriter.h
    ${TOOLSLIB_INCLUDE_DIR}/script/format/pcodereader.h
    ${TOOLSLIB_INCLUDE_DIR}/script/format/pcodewriter.h
    ${TOOLSLIB_INCLUDE_DIR}/ssf.h
    ${TOOLSLIB_INCLUDE_DIR}/tlk.h
    ${TOOLSLIB_INCLUDE_DIR}/tool.h
    ${TOOLSLIB_INCLUDE_DIR}/tpc.h
    ${TOOLSLIB_INCLUDE_DIR}/types.h)

set(TOOLSLIB_SOURCES
    ${TOOLSLIB_SOURCE_DIR}/script/expressiontree.cpp
    ${TOOLSLIB_SOURCE_DIR}/script/format/nsswriter.cpp
    ${TOOLSLIB_SOURCE_DIR}/script/format/pcodereader.cpp
    ${TOOLSLIB_SOURCE_DIR}/script/format/pcodewriter.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/2da.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/audio.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/erf.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/gff.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/keybif.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/lip.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/ncs.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/rim.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/ssf.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/tlk.cpp
    ${TOOLSLIB_SOURCE_DIR}/tool/tpc.cpp)

add_library(toolslib ${TOOLSLIB_HEADERS} ${TOOLSLIB_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(toolslib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(toolslib PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(toolslib PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(toolslib PUBLIC game tinyxml2 ${Boost_REGEX_LIBRARY})
