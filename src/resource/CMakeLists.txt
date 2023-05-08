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

set(RESOURCE_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/resource)

set(RESOURCE_HEADERS
    ${RESOURCE_INCLUDE_DIR}/2da.h
    ${RESOURCE_INCLUDE_DIR}/2das.h
    ${RESOURCE_INCLUDE_DIR}/gff.h
    ${RESOURCE_INCLUDE_DIR}/gffs.h
    ${RESOURCE_INCLUDE_DIR}/format/2dareader.h
    ${RESOURCE_INCLUDE_DIR}/format/2dawriter.h
    ${RESOURCE_INCLUDE_DIR}/format/bifreader.h
    ${RESOURCE_INCLUDE_DIR}/format/binreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/gffreader.h
    ${RESOURCE_INCLUDE_DIR}/format/gffwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/keyreader.h
    ${RESOURCE_INCLUDE_DIR}/format/pereader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimreader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkreader.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkwriter.h
    ${RESOURCE_INCLUDE_DIR}/id.h
    ${RESOURCE_INCLUDE_DIR}/provider.h
    ${RESOURCE_INCLUDE_DIR}/provider/erf.h
    ${RESOURCE_INCLUDE_DIR}/provider/folder.h
    ${RESOURCE_INCLUDE_DIR}/provider/keybif.h
    ${RESOURCE_INCLUDE_DIR}/provider/rim.h
    ${RESOURCE_INCLUDE_DIR}/resources.h
    ${RESOURCE_INCLUDE_DIR}/services.h
    ${RESOURCE_INCLUDE_DIR}/strings.h
    ${RESOURCE_INCLUDE_DIR}/talktable.h
    ${RESOURCE_INCLUDE_DIR}/types.h
    ${RESOURCE_INCLUDE_DIR}/typeutil.h)

set(RESOURCE_SOURCES
    2da.cpp
    2das.cpp
    gff.cpp
    gffs.cpp
    format/2dareader.cpp
    format/2dawriter.cpp
    format/bifreader.cpp
    format/binreader.cpp
    format/erfreader.cpp
    format/erfwriter.cpp
    format/gffreader.cpp
    format/gffwriter.cpp
    format/keyreader.cpp
    format/pereader.cpp
    format/rimreader.cpp
    format/rimwriter.cpp
    format/tlkreader.cpp
    format/tlkwriter.cpp
    provider/erf.cpp
    provider/folder.cpp
    provider/keybif.cpp
    provider/rim.cpp
    resources.cpp
    strings.cpp
    talktable.cpp
    typeutil.cpp)

add_library(resource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(resource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(resource PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(resource PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
