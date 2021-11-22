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

set(RESOURCE_HEADERS
    2da.h
    2das.h
    gffs.h
    gffstruct.h
    folder.h
    format/2dareader.h
    format/2dawriter.h
    format/bifreader.h
    format/binreader.h
    format/erfreader.h
    format/erfwriter.h
    format/gffreader.h
    format/gffwriter.h
    format/keyreader.h
    format/ltrreader.h
    format/lytreader.h
    format/pereader.h
    format/rimreader.h
    format/rimwriter.h
    format/ssfreader.h
    format/ssfwriter.h
    format/tlkreader.h
    format/tlkwriter.h
    format/visreader.h
    id.h
    keybifprovider.h
    resourceprovider.h
    resources.h
    strings.h
    talktable.h
    types.h
    typeutil.h)

set(RESOURCE_SOURCES
    2da.cpp
    2das.cpp
    gfffield.cpp
    gffs.cpp
    gffstruct.cpp
    folder.cpp
    format/2dareader.cpp
    format/2dawriter.cpp
    format/bifreader.cpp
    format/binreader.cpp
    format/erfreader.cpp
    format/erfwriter.cpp
    format/gffreader.cpp
    format/gffwriter.cpp
    format/keyreader.cpp
    format/ltrreader.cpp
    format/lytreader.cpp
    format/pereader.cpp
    format/rimreader.cpp
    format/rimwriter.cpp
    format/ssfreader.cpp
    format/ssfwriter.cpp
    format/tlkreader.cpp
    format/tlkwriter.cpp
    format/visreader.cpp
    keybifprovider.cpp
    resources.cpp
    strings.cpp
    talktable.cpp
    typeutil.cpp)

add_library(libresource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES})
set_target_properties(libresource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libresource PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)
