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

set(ANALYZER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/analyzer)

set(ANALYZER_HEADERS
    ${ANALYZER_SOURCE_DIR}/code.h
    ${ANALYZER_SOURCE_DIR}/guis.h
    ${ANALYZER_SOURCE_DIR}/models.h
    ${ANALYZER_SOURCE_DIR}/routines.h
    ${ANALYZER_SOURCE_DIR}/templates.h)

set(ANALYZER_SOURCES
    ${ANALYZER_SOURCE_DIR}/guis.cpp
    ${ANALYZER_SOURCE_DIR}/main.cpp
    ${ANALYZER_SOURCE_DIR}/models.cpp
    ${ANALYZER_SOURCE_DIR}/routines.cpp
    ${ANALYZER_SOURCE_DIR}/templates.cpp)

add_executable(analyzer ${ANALYZER_SOURCES} ${ANALYZER_HEADERS} ${CLANG_FORMAT_PATH})
set_target_properties(analyzer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(analyzer PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(analyzer PRIVATE resource movie graphics audio script system ${Boost_PROGRAM_OPTIONS_LIBRARY})
