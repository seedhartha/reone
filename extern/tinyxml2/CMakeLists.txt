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

set(TINYXML2_SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/tinyxml2)

add_library(tinyxml2 STATIC ${TINYXML2_SOURCE_DIR}/tinyxml2.h ${TINYXML2_SOURCE_DIR}/tinyxml2.cpp)
set_target_properties(tinyxml2 PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(tinyxml2 PROPERTIES DEBUG_POSTFIX "d")
