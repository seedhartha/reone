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

find_package(Boost COMPONENTS regex REQUIRED)

set(TOOLSLIB_HEADERS
    script/expressiontree.h
    script/format/nsswriter.h
    script/format/pcodereader.h
    script/format/pcodewriter.h
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

set(TOOLSLIB_SOURCES
    script/expressiontree.cpp
    script/format/nsswriter.cpp
    script/format/pcodereader.cpp
    script/format/pcodewriter.cpp
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

add_library(toolslib ${TOOLSLIB_HEADERS} ${TOOLSLIB_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(toolslib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(toolslib PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(toolslib PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(toolslib PUBLIC game tinyxml2 ${Boost_REGEX_LIBRARY})
