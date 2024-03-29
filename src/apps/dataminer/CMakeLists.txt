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

set(DATAMINER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/apps/dataminer)

set(DATAMINER_HEADERS
    ${DATAMINER_SOURCE_DIR}/2daparsers.h
    ${DATAMINER_SOURCE_DIR}/code.h
    ${DATAMINER_SOURCE_DIR}/gffparsers.h
    ${DATAMINER_SOURCE_DIR}/guis.h
    ${DATAMINER_SOURCE_DIR}/models.h
    ${DATAMINER_SOURCE_DIR}/routines.h)

set(DATAMINER_SOURCES
    ${DATAMINER_SOURCE_DIR}/2daparsers.cpp
    ${DATAMINER_SOURCE_DIR}/gffparsers.cpp
    ${DATAMINER_SOURCE_DIR}/guis.cpp
    ${DATAMINER_SOURCE_DIR}/main.cpp
    ${DATAMINER_SOURCE_DIR}/models.cpp
    ${DATAMINER_SOURCE_DIR}/routines.cpp)

add_executable(dataminer ${DATAMINER_SOURCES} ${DATAMINER_HEADERS} ${CLANG_FORMAT_PATH})
set_target_properties(dataminer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_precompile_headers(dataminer PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(dataminer PRIVATE resource movie graphics audio script system ${Boost_PROGRAM_OPTIONS_LIBRARY})
