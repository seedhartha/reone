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

set(RESOURCE_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/resource)
set(RESOURCE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/resource)

set(RESOURCE_HEADERS
    ${RESOURCE_INCLUDE_DIR}/2da.h
    ${RESOURCE_INCLUDE_DIR}/2das.h
    ${RESOURCE_INCLUDE_DIR}/audio/files.h
    ${RESOURCE_INCLUDE_DIR}/audio/player.h
    ${RESOURCE_INCLUDE_DIR}/container.h
    ${RESOURCE_INCLUDE_DIR}/container/erf.h
    ${RESOURCE_INCLUDE_DIR}/container/exe.h
    ${RESOURCE_INCLUDE_DIR}/container/folder.h
    ${RESOURCE_INCLUDE_DIR}/container/keybif.h
    ${RESOURCE_INCLUDE_DIR}/container/memory.h
    ${RESOURCE_INCLUDE_DIR}/container/rim.h
    ${RESOURCE_INCLUDE_DIR}/di/module.h
    ${RESOURCE_INCLUDE_DIR}/di/services.h
    ${RESOURCE_INCLUDE_DIR}/exception/format.h
    ${RESOURCE_INCLUDE_DIR}/exception/notfound.h
    ${RESOURCE_INCLUDE_DIR}/format/2dareader.h
    ${RESOURCE_INCLUDE_DIR}/format/2dawriter.h
    ${RESOURCE_INCLUDE_DIR}/format/bifreader.h
    ${RESOURCE_INCLUDE_DIR}/format/bikreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/gffreader.h
    ${RESOURCE_INCLUDE_DIR}/format/gffwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/keyreader.h
    ${RESOURCE_INCLUDE_DIR}/format/ltrreader.h
    ${RESOURCE_INCLUDE_DIR}/format/lytreader.h
    ${RESOURCE_INCLUDE_DIR}/format/mp3reader.h
    ${RESOURCE_INCLUDE_DIR}/format/ncsreader.h
    ${RESOURCE_INCLUDE_DIR}/format/ncswriter.h
    ${RESOURCE_INCLUDE_DIR}/format/pereader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimreader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/signutil.h
    ${RESOURCE_INCLUDE_DIR}/format/ssfreader.h
    ${RESOURCE_INCLUDE_DIR}/format/ssfwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkreader.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/visreader.h
    ${RESOURCE_INCLUDE_DIR}/format/wavreader.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/are.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/dlg.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/git.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/gui.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/ifo.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/pth.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/utc.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/utd.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/ute.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/uti.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/utm.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/utp.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/uts.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/utt.h
    ${RESOURCE_INCLUDE_DIR}/template/generated/utw.h
    ${RESOURCE_INCLUDE_DIR}/gff.h
    ${RESOURCE_INCLUDE_DIR}/gffs.h
    ${RESOURCE_INCLUDE_DIR}/id.h
    ${RESOURCE_INCLUDE_DIR}/layout.h
    ${RESOURCE_INCLUDE_DIR}/movies.h
    ${RESOURCE_INCLUDE_DIR}/resource.h
    ${RESOURCE_INCLUDE_DIR}/resources.h
    ${RESOURCE_INCLUDE_DIR}/resref.h
    ${RESOURCE_INCLUDE_DIR}/scripts.h
    ${RESOURCE_INCLUDE_DIR}/strings.h
    ${RESOURCE_INCLUDE_DIR}/talktable.h
    ${RESOURCE_INCLUDE_DIR}/types.h
    ${RESOURCE_INCLUDE_DIR}/typeutil.h)

set(RESOURCE_SOURCES
    ${RESOURCE_SOURCE_DIR}/2da.cpp
    ${RESOURCE_SOURCE_DIR}/2das.cpp
    ${RESOURCE_SOURCE_DIR}/audio/files.cpp
    ${RESOURCE_SOURCE_DIR}/audio/player.cpp
    ${RESOURCE_SOURCE_DIR}/container/erf.cpp
    ${RESOURCE_SOURCE_DIR}/container/exe.cpp
    ${RESOURCE_SOURCE_DIR}/container/folder.cpp
    ${RESOURCE_SOURCE_DIR}/container/keybif.cpp
    ${RESOURCE_SOURCE_DIR}/container/rim.cpp
    ${RESOURCE_SOURCE_DIR}/di/module.cpp
    ${RESOURCE_SOURCE_DIR}/format/2dareader.cpp
    ${RESOURCE_SOURCE_DIR}/format/2dawriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/bifreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/bikreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/erfreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/erfwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/gffreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/gffwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/keyreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/ltrreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/lytreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/mp3reader.cpp
    ${RESOURCE_SOURCE_DIR}/format/ncsreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/ncswriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/pereader.cpp
    ${RESOURCE_SOURCE_DIR}/format/rimreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/rimwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/signutil.cpp
    ${RESOURCE_SOURCE_DIR}/format/ssfreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/ssfwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/tlkreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/tlkwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/visreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/wavreader.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/are.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/dlg.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/git.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/gui.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/ifo.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/pth.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/utc.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/utd.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/ute.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/uti.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/utm.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/utp.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/uts.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/utt.cpp
    ${RESOURCE_SOURCE_DIR}/template/generated/utw.cpp
    ${RESOURCE_SOURCE_DIR}/gff.cpp
    ${RESOURCE_SOURCE_DIR}/gffs.cpp
    ${RESOURCE_SOURCE_DIR}/movies.cpp
    ${RESOURCE_SOURCE_DIR}/resources.cpp
    ${RESOURCE_SOURCE_DIR}/scripts.cpp
    ${RESOURCE_SOURCE_DIR}/strings.cpp
    ${RESOURCE_SOURCE_DIR}/talktable.cpp
    ${RESOURCE_SOURCE_DIR}/typeutil.cpp)

add_library(resource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(resource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(resource PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(resource PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
