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

set(INPUT_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/input)
set(INPUT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/input)

set(INPUT_HEADERS
    ${INPUT_INCLUDE_DIR}/event.h)

add_library(input INTERFACE ${INPUT_HEADERS} ${CLANG_FORMAT_PATH})
set_target_properties(input PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(input PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(input INTERFACE ${CMAKE_SOURCE_DIR}/src/pch.h)
