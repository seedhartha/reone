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

set(GAME_HEADERS
    probe.h
    types.h)

set(GAME_SOURCES
    probe.cpp)

add_library(game STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(game PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(game PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)
target_link_libraries(game PUBLIC limbo tsl)
