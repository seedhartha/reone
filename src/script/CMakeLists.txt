# Copyright (c) 2020-2022 The reone project contributors

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

set(SCRIPT_HEADERS
    enginetype.h
    exception/argument.h
    exception/notimpl.h
    exception/unsupportedroutine.h
    execution.h
    executioncontext.h
    executionstate.h
    expressiontree.h
    format/ncsreader.h
    format/ncswriter.h
    format/nsswriter.h
    format/pcodereader.h
    format/pcodewriter.h
    instrutil.h
    program.h
    routine.h
    routines.h
    scripts.h
    services.h
    types.h
    variable.h
    variableutil.h)

set(SCRIPT_SOURCES
    execution.cpp
    expressiontree.cpp
    format/ncsreader.cpp
    format/ncswriter.cpp
    format/nsswriter.cpp
    format/pcodereader.cpp
    format/pcodewriter.cpp
    instrutil.cpp
    program.cpp
    routine.cpp
    scripts.cpp
    variable.cpp
    variableutil.cpp)

add_library(script STATIC ${SCRIPT_HEADERS} ${SCRIPT_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(script PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(script PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
