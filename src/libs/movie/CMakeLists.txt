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

set(MOVIE_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/movie)
set(MOVIE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/movie)

set(MOVIE_HEADERS
    ${MOVIE_INCLUDE_DIR}/di/module.h
    ${MOVIE_INCLUDE_DIR}/di/services.h
    ${MOVIE_INCLUDE_DIR}/format/bikreader.h
    ${MOVIE_INCLUDE_DIR}/movie.h
    ${MOVIE_INCLUDE_DIR}/videostream.h)

set(MOVIE_SOURCES
    ${MOVIE_SOURCE_DIR}/di/module.cpp
    ${MOVIE_SOURCE_DIR}/format/bikreader.cpp
    ${MOVIE_SOURCE_DIR}/movie.cpp)

add_library(movie STATIC ${MOVIE_HEADERS} ${MOVIE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(movie PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(movie PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(movie PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)

if(ENABLE_MOVIE)
    target_compile_definitions(movie PRIVATE R_ENABLE_MOVIE)
    target_link_libraries(movie PRIVATE ${FFMPEG_LIBRARIES})
endif()
