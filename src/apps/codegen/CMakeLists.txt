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

set(CODEGEN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/codegen)

set(CODEGEN_SOURCES
    ${CODEGEN_SOURCE_DIR}/gffschema.cpp
    ${CODEGEN_SOURCE_DIR}/guis.cpp
    ${CODEGEN_SOURCE_DIR}/main.cpp
    ${CODEGEN_SOURCE_DIR}/routines.cpp)

set(CODEGEN_HEADERS
    ${CODEGEN_SOURCE_DIR}/gffschema.h
    ${CODEGEN_SOURCE_DIR}/guis.h
    ${CODEGEN_SOURCE_DIR}/routines.h
    ${CODEGEN_SOURCE_DIR}/templates.h)

add_executable(codegen ${CODEGEN_SOURCES} ${CODEGEN_HEADERS} ${CLANG_FORMAT_PATH})
set_target_properties(codegen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(codegen PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(codegen PRIVATE tools resource system ${Boost_PROGRAM_OPTIONS_LIBRARY})
