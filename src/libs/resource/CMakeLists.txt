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

set(RESOURCE_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/resource)
set(RESOURCE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/resource)

set(RESOURCE_HEADERS
    ${RESOURCE_INCLUDE_DIR}/2da.h
    ${RESOURCE_INCLUDE_DIR}/2das.h
    ${RESOURCE_INCLUDE_DIR}/di/module.h
    ${RESOURCE_INCLUDE_DIR}/di/services.h
    ${RESOURCE_INCLUDE_DIR}/exception/format.h
    ${RESOURCE_INCLUDE_DIR}/exception/notfound.h
    ${RESOURCE_INCLUDE_DIR}/format/2dareader.h
    ${RESOURCE_INCLUDE_DIR}/format/2dawriter.h
    ${RESOURCE_INCLUDE_DIR}/format/bifreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/gffreader.h
    ${RESOURCE_INCLUDE_DIR}/format/gffwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/keyreader.h
    ${RESOURCE_INCLUDE_DIR}/format/pereader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimreader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/signutil.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkreader.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkwriter.h
    ${RESOURCE_INCLUDE_DIR}/gff.h
    ${RESOURCE_INCLUDE_DIR}/gffs.h
    ${RESOURCE_INCLUDE_DIR}/id.h
    ${RESOURCE_INCLUDE_DIR}/provider.h
    ${RESOURCE_INCLUDE_DIR}/provider/erf.h
    ${RESOURCE_INCLUDE_DIR}/provider/exe.h
    ${RESOURCE_INCLUDE_DIR}/provider/folder.h
    ${RESOURCE_INCLUDE_DIR}/provider/memory.h
    ${RESOURCE_INCLUDE_DIR}/provider/keybif.h
    ${RESOURCE_INCLUDE_DIR}/provider/rim.h
    ${RESOURCE_INCLUDE_DIR}/resources.h
    ${RESOURCE_INCLUDE_DIR}/strings.h
    ${RESOURCE_INCLUDE_DIR}/talktable.h
    ${RESOURCE_INCLUDE_DIR}/types.h
    ${RESOURCE_INCLUDE_DIR}/typeutil.h)

set(RESOURCE_SOURCES
    ${RESOURCE_SOURCE_DIR}/2da.cpp
    ${RESOURCE_SOURCE_DIR}/2das.cpp
    ${RESOURCE_SOURCE_DIR}/di/module.cpp
    ${RESOURCE_SOURCE_DIR}/format/2dareader.cpp
    ${RESOURCE_SOURCE_DIR}/format/2dawriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/bifreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/erfreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/erfwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/gffreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/gffwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/keyreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/pereader.cpp
    ${RESOURCE_SOURCE_DIR}/format/rimreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/rimwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/signutil.cpp
    ${RESOURCE_SOURCE_DIR}/format/tlkreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/tlkwriter.cpp
    ${RESOURCE_SOURCE_DIR}/gff.cpp
    ${RESOURCE_SOURCE_DIR}/gffs.cpp
    ${RESOURCE_SOURCE_DIR}/provider/erf.cpp
    ${RESOURCE_SOURCE_DIR}/provider/exe.cpp
    ${RESOURCE_SOURCE_DIR}/provider/folder.cpp
    ${RESOURCE_SOURCE_DIR}/provider/keybif.cpp
    ${RESOURCE_SOURCE_DIR}/provider/rim.cpp
    ${RESOURCE_SOURCE_DIR}/resources.cpp
    ${RESOURCE_SOURCE_DIR}/strings.cpp
    ${RESOURCE_SOURCE_DIR}/talktable.cpp
    ${RESOURCE_SOURCE_DIR}/typeutil.cpp)

add_library(resource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(resource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(resource PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(resource PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
