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

set(COMMON_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/common)
set(COMMON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/common)

set(COMMON_HEADERS
    ${COMMON_INCLUDE_DIR}/binaryreader.h
    ${COMMON_INCLUDE_DIR}/binarywriter.h
    ${COMMON_INCLUDE_DIR}/collectionutil.h
    ${COMMON_INCLUDE_DIR}/exception/argument.h
    ${COMMON_INCLUDE_DIR}/exception/notfound.h
    ${COMMON_INCLUDE_DIR}/exception/notimplemented.h
    ${COMMON_INCLUDE_DIR}/exception/validation.h
    ${COMMON_INCLUDE_DIR}/hexutil.h
    ${COMMON_INCLUDE_DIR}/logutil.h
    ${COMMON_INCLUDE_DIR}/memorycache.h
    ${COMMON_INCLUDE_DIR}/pathutil.h
    ${COMMON_INCLUDE_DIR}/randomutil.h
    ${COMMON_INCLUDE_DIR}/stream/bytearrayinput.h
    ${COMMON_INCLUDE_DIR}/stream/bytearrayoutput.h
    ${COMMON_INCLUDE_DIR}/stream/fileinput.h
    ${COMMON_INCLUDE_DIR}/stream/fileoutput.h
    ${COMMON_INCLUDE_DIR}/stream/input.h
    ${COMMON_INCLUDE_DIR}/stream/output.h
    ${COMMON_INCLUDE_DIR}/stringbuilder.h
    ${COMMON_INCLUDE_DIR}/textwriter.h
    ${COMMON_INCLUDE_DIR}/timer.h
    ${COMMON_INCLUDE_DIR}/types.h)

set(COMMON_SOURCES
    ${COMMON_SOURCE_DIR}/binaryreader.cpp
    ${COMMON_SOURCE_DIR}/binarywriter.cpp
    ${COMMON_SOURCE_DIR}/hexutil.cpp
    ${COMMON_SOURCE_DIR}/logutil.cpp
    ${COMMON_SOURCE_DIR}/pathutil.cpp
    ${COMMON_SOURCE_DIR}/randomutil.cpp
    ${COMMON_SOURCE_DIR}/textwriter.cpp)

add_library(common STATIC ${COMMON_HEADERS} ${COMMON_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(common PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(common PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(common PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(common PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

if(NOT MSVC)
    target_link_libraries(common PRIVATE Threads::Threads)
endif()
