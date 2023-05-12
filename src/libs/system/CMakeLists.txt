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

set(SYSTEM_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/system)
set(SYSTEM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/system)

set(SYSTEM_HEADERS
    ${SYSTEM_INCLUDE_DIR}/binaryreader.h
    ${SYSTEM_INCLUDE_DIR}/binarywriter.h
    ${SYSTEM_INCLUDE_DIR}/clock.h
    ${SYSTEM_INCLUDE_DIR}/collectionutil.h
    ${SYSTEM_INCLUDE_DIR}/exception/argument.h
    ${SYSTEM_INCLUDE_DIR}/exception/notfound.h
    ${SYSTEM_INCLUDE_DIR}/exception/notimplemented.h
    ${SYSTEM_INCLUDE_DIR}/exception/validation.h
    ${SYSTEM_INCLUDE_DIR}/hexutil.h
    ${SYSTEM_INCLUDE_DIR}/logutil.h
    ${SYSTEM_INCLUDE_DIR}/memorycache.h
    ${SYSTEM_INCLUDE_DIR}/pathutil.h
    ${SYSTEM_INCLUDE_DIR}/randomutil.h
    ${SYSTEM_INCLUDE_DIR}/services.h
    ${SYSTEM_INCLUDE_DIR}/stream/bytearrayinput.h
    ${SYSTEM_INCLUDE_DIR}/stream/bytearrayoutput.h
    ${SYSTEM_INCLUDE_DIR}/stream/fileinput.h
    ${SYSTEM_INCLUDE_DIR}/stream/fileoutput.h
    ${SYSTEM_INCLUDE_DIR}/stream/input.h
    ${SYSTEM_INCLUDE_DIR}/stream/output.h
    ${SYSTEM_INCLUDE_DIR}/stringbuilder.h
    ${SYSTEM_INCLUDE_DIR}/textwriter.h
    ${SYSTEM_INCLUDE_DIR}/timer.h
    ${SYSTEM_INCLUDE_DIR}/types.h)

set(SYSTEM_SOURCES
    ${SYSTEM_SOURCE_DIR}/binaryreader.cpp
    ${SYSTEM_SOURCE_DIR}/binarywriter.cpp
    ${SYSTEM_SOURCE_DIR}/clock.cpp
    ${SYSTEM_SOURCE_DIR}/hexutil.cpp
    ${SYSTEM_SOURCE_DIR}/logutil.cpp
    ${SYSTEM_SOURCE_DIR}/pathutil.cpp
    ${SYSTEM_SOURCE_DIR}/randomutil.cpp
    ${SYSTEM_SOURCE_DIR}/textwriter.cpp)

add_library(system STATIC ${SYSTEM_HEADERS} ${SYSTEM_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(system PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(system PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(system PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(system PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

if(NOT MSVC)
    target_link_libraries(system PRIVATE Threads::Threads)
endif()