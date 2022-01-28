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

set(TOOLS_HEADERS
    program.h
    tool.h
    tool/2da.h
    tool/audio.h
    tool/erf.h
    tool/gff.h
    tool/keybif.h
    tool/lip.h
    tool/ncs.h
    tool/rim.h
    tool/ssf.h
    tool/tlk.h
    tool/tpc.h
    types.h)

set(TOOLS_SOURCES
    main.cpp
    program.cpp
    tool/2da.cpp
    tool/audio.cpp
    tool/erf.cpp
    tool/gff.cpp
    tool/keybif.cpp
    tool/lip.cpp
    tool/ncs.cpp
    tool/rim.cpp
    tool/ssf.cpp
    tool/tlk.cpp
    tool/tpc.cpp)

add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_precompile_headers(reone-tools PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone-tools PRIVATE kotor ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_REGEX_LIBRARY})

list(APPEND InstallTargets reone-tools)
