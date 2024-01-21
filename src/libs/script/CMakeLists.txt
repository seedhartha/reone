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

set(SCRIPT_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/script)
set(SCRIPT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/script)

set(SCRIPT_HEADERS
    ${SCRIPT_INCLUDE_DIR}/di/module.h
    ${SCRIPT_INCLUDE_DIR}/di/services.h
    ${SCRIPT_INCLUDE_DIR}/enginetype.h
    ${SCRIPT_INCLUDE_DIR}/executioncontext.h
    ${SCRIPT_INCLUDE_DIR}/executionstate.h
    ${SCRIPT_INCLUDE_DIR}/format/ncsreader.h
    ${SCRIPT_INCLUDE_DIR}/format/ncswriter.h
    ${SCRIPT_INCLUDE_DIR}/instrutil.h
    ${SCRIPT_INCLUDE_DIR}/program.h
    ${SCRIPT_INCLUDE_DIR}/routine.h
    ${SCRIPT_INCLUDE_DIR}/routine/exception/argmissing.h
    ${SCRIPT_INCLUDE_DIR}/routine/exception/argument.h
    ${SCRIPT_INCLUDE_DIR}/routine/exception/notimplemented.h
    ${SCRIPT_INCLUDE_DIR}/routines.h
    ${SCRIPT_INCLUDE_DIR}/types.h
    ${SCRIPT_INCLUDE_DIR}/variable.h
    ${SCRIPT_INCLUDE_DIR}/variableutil.h
    ${SCRIPT_INCLUDE_DIR}/virtualmachine.h)

set(SCRIPT_SOURCES
    ${SCRIPT_SOURCE_DIR}/di/module.cpp
    ${SCRIPT_SOURCE_DIR}/format/ncsreader.cpp
    ${SCRIPT_SOURCE_DIR}/format/ncswriter.cpp
    ${SCRIPT_SOURCE_DIR}/instrutil.cpp
    ${SCRIPT_SOURCE_DIR}/program.cpp
    ${SCRIPT_SOURCE_DIR}/routine.cpp
    ${SCRIPT_SOURCE_DIR}/variable.cpp
    ${SCRIPT_SOURCE_DIR}/variableutil.cpp
    ${SCRIPT_SOURCE_DIR}/virtualmachine.cpp)

add_library(script STATIC ${SCRIPT_HEADERS} ${SCRIPT_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(script PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(script PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(script PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
