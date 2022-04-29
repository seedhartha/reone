# Copyright (c) 2020-2021 The reone project contributors

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

set(COMMON_HEADERS
    collectionutil.h
    exception/validation.h
    hexutil.h
    logutil.h
    memorycache.h
    pathutil.h
    randomutil.h
    streamreader.h
    streamutil.h
    streamwriter.h
    timer.h
    types.h
    unicodeutil.h)

set(COMMON_SOURCES
    hexutil.cpp
    logutil.cpp
    pathutil.cpp
    randomutil.cpp
    streamreader.cpp
    streamutil.cpp
    streamwriter.cpp
    unicodeutil.cpp)

add_library(common STATIC ${COMMON_HEADERS} ${COMMON_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(common PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(common PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(common PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

if(NOT MSVC)
    target_link_libraries(common PRIVATE Threads::Threads -latomic)
endif()
