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
    ${RESOURCE_INCLUDE_DIR}/container.h
    ${RESOURCE_INCLUDE_DIR}/container/erf.h
    ${RESOURCE_INCLUDE_DIR}/container/exe.h
    ${RESOURCE_INCLUDE_DIR}/container/folder.h
    ${RESOURCE_INCLUDE_DIR}/container/keybif.h
    ${RESOURCE_INCLUDE_DIR}/container/memory.h
    ${RESOURCE_INCLUDE_DIR}/container/rim.h
    ${RESOURCE_INCLUDE_DIR}/di/module.h
    ${RESOURCE_INCLUDE_DIR}/di/services.h
    ${RESOURCE_INCLUDE_DIR}/dialog.h
    ${RESOURCE_INCLUDE_DIR}/director.h
    ${RESOURCE_INCLUDE_DIR}/exception/notfound.h
    ${RESOURCE_INCLUDE_DIR}/format/2dareader.h
    ${RESOURCE_INCLUDE_DIR}/format/2dawriter.h
    ${RESOURCE_INCLUDE_DIR}/format/bifreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfreader.h
    ${RESOURCE_INCLUDE_DIR}/format/erfwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/gffreader.h
    ${RESOURCE_INCLUDE_DIR}/format/gffwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/keyreader.h
    ${RESOURCE_INCLUDE_DIR}/format/ltrreader.h
    ${RESOURCE_INCLUDE_DIR}/format/lytreader.h
    ${RESOURCE_INCLUDE_DIR}/format/pereader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimreader.h
    ${RESOURCE_INCLUDE_DIR}/format/rimwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/ssfreader.h
    ${RESOURCE_INCLUDE_DIR}/format/ssfwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkreader.h
    ${RESOURCE_INCLUDE_DIR}/format/tlkwriter.h
    ${RESOURCE_INCLUDE_DIR}/format/visreader.h
    ${RESOURCE_INCLUDE_DIR}/gameprobe.h
    ${RESOURCE_INCLUDE_DIR}/gff.h
    ${RESOURCE_INCLUDE_DIR}/id.h
    ${RESOURCE_INCLUDE_DIR}/layout.h
    ${RESOURCE_INCLUDE_DIR}/ltr.h
    ${RESOURCE_INCLUDE_DIR}/parser/2da/appearance.h
    ${RESOURCE_INCLUDE_DIR}/parser/2da/genericdoors.h
    ${RESOURCE_INCLUDE_DIR}/parser/2da/heads.h
    ${RESOURCE_INCLUDE_DIR}/parser/2da/placeables.h
    ${RESOURCE_INCLUDE_DIR}/parser/2da/portraits.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/are.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/dlg.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/git.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/gui.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/ifo.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/pth.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/utc.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/utd.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/ute.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/uti.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/utm.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/utp.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/uts.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/utt.h
    ${RESOURCE_INCLUDE_DIR}/parser/gff/utw.h
    ${RESOURCE_INCLUDE_DIR}/path.h
    ${RESOURCE_INCLUDE_DIR}/provider/2das.h
    ${RESOURCE_INCLUDE_DIR}/provider/audioclips.h
    ${RESOURCE_INCLUDE_DIR}/provider/cursors.h
    ${RESOURCE_INCLUDE_DIR}/provider/dialogs.h
    ${RESOURCE_INCLUDE_DIR}/provider/fonts.h
    ${RESOURCE_INCLUDE_DIR}/provider/gffs.h
    ${RESOURCE_INCLUDE_DIR}/provider/layouts.h
    ${RESOURCE_INCLUDE_DIR}/provider/lips.h
    ${RESOURCE_INCLUDE_DIR}/provider/ltrs.h
    ${RESOURCE_INCLUDE_DIR}/provider/models.h
    ${RESOURCE_INCLUDE_DIR}/provider/movies.h
    ${RESOURCE_INCLUDE_DIR}/provider/paths.h
    ${RESOURCE_INCLUDE_DIR}/provider/scripts.h
    ${RESOURCE_INCLUDE_DIR}/provider/shaders.h
    ${RESOURCE_INCLUDE_DIR}/provider/soundsets.h
    ${RESOURCE_INCLUDE_DIR}/provider/textures.h
    ${RESOURCE_INCLUDE_DIR}/provider/visibilities.h
    ${RESOURCE_INCLUDE_DIR}/provider/walkmeshes.h
    ${RESOURCE_INCLUDE_DIR}/resource.h
    ${RESOURCE_INCLUDE_DIR}/resources.h
    ${RESOURCE_INCLUDE_DIR}/resref.h
    ${RESOURCE_INCLUDE_DIR}/strings.h
    ${RESOURCE_INCLUDE_DIR}/talktable.h
    ${RESOURCE_INCLUDE_DIR}/types.h
    ${RESOURCE_INCLUDE_DIR}/typeutil.h)

set(RESOURCE_SOURCES
    ${RESOURCE_SOURCE_DIR}/2da.cpp
    ${RESOURCE_SOURCE_DIR}/container/erf.cpp
    ${RESOURCE_SOURCE_DIR}/container/exe.cpp
    ${RESOURCE_SOURCE_DIR}/container/folder.cpp
    ${RESOURCE_SOURCE_DIR}/container/keybif.cpp
    ${RESOURCE_SOURCE_DIR}/container/rim.cpp
    ${RESOURCE_SOURCE_DIR}/di/module.cpp
    ${RESOURCE_SOURCE_DIR}/director.cpp
    ${RESOURCE_SOURCE_DIR}/format/2dareader.cpp
    ${RESOURCE_SOURCE_DIR}/format/2dawriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/bifreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/erfreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/erfwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/gffreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/gffwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/keyreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/ltrreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/lytreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/pereader.cpp
    ${RESOURCE_SOURCE_DIR}/format/rimreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/rimwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/ssfreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/ssfwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/tlkreader.cpp
    ${RESOURCE_SOURCE_DIR}/format/tlkwriter.cpp
    ${RESOURCE_SOURCE_DIR}/format/visreader.cpp
    ${RESOURCE_SOURCE_DIR}/gameprobe.cpp
    ${RESOURCE_SOURCE_DIR}/gff.cpp
    ${RESOURCE_SOURCE_DIR}/ltr.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/are.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/dlg.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/git.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/gui.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/ifo.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/pth.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/utc.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/utd.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/ute.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/uti.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/utm.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/utp.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/uts.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/utt.cpp
    ${RESOURCE_SOURCE_DIR}/parser/gff/utw.cpp
    ${RESOURCE_SOURCE_DIR}/provider/2das.cpp
    ${RESOURCE_SOURCE_DIR}/provider/audioclips.cpp
    ${RESOURCE_SOURCE_DIR}/provider/cursors.cpp
    ${RESOURCE_SOURCE_DIR}/provider/dialogs.cpp
    ${RESOURCE_SOURCE_DIR}/provider/fonts.cpp
    ${RESOURCE_SOURCE_DIR}/provider/gffs.cpp
    ${RESOURCE_SOURCE_DIR}/provider/layouts.cpp
    ${RESOURCE_SOURCE_DIR}/provider/lips.cpp
    ${RESOURCE_SOURCE_DIR}/provider/models.cpp
    ${RESOURCE_SOURCE_DIR}/provider/movies.cpp
    ${RESOURCE_SOURCE_DIR}/provider/paths.cpp
    ${RESOURCE_SOURCE_DIR}/provider/scripts.cpp
    ${RESOURCE_SOURCE_DIR}/provider/shaders.cpp
    ${RESOURCE_SOURCE_DIR}/provider/soundsets.cpp
    ${RESOURCE_SOURCE_DIR}/provider/textures.cpp
    ${RESOURCE_SOURCE_DIR}/provider/visibilities.cpp
    ${RESOURCE_SOURCE_DIR}/provider/walkmeshes.cpp
    ${RESOURCE_SOURCE_DIR}/resources.cpp
    ${RESOURCE_SOURCE_DIR}/strings.cpp
    ${RESOURCE_SOURCE_DIR}/talktable.cpp
    ${RESOURCE_SOURCE_DIR}/typeutil.cpp)

add_library(resource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(resource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(resource PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(resource PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
