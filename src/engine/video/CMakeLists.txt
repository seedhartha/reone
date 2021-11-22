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

set(VIDEO_HEADERS
    format/bikreader.h
    video.h)

set(VIDEO_SOURCES
    format/bikreader.cpp
    video.cpp)

add_library(libvideo STATIC ${VIDEO_HEADERS} ${VIDEO_SOURCES})
set_target_properties(libvideo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libvideo PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)

if(ENABLE_VIDEO)
    target_compile_definitions(libvideo PRIVATE R_ENABLE_VIDEO)
    target_link_libraries(libvideo PUBLIC ${FFMPEG_LIBRARIES})
endif()
