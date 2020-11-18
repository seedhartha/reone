# Copyright (c) 2020 The reone project contributors

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

find_path(MAD_INCLUDE_DIR mad.h DOC "MAD include directory")
find_library(MAD_LIBRARY NAMES mad DOC "MAD library")

if(MAD_INCLUDE_DIR AND MAD_LIBRARY)
    set(MAD_FOUND 1)
    set(MAD_LIBRARIES ${MAD_LIBRARY})
    set(MAD_INCLUDE_DIRS ${MAD_INCLUDE_DIR})
else()
    set(MAD_FOUND 0)
    set(MAD_LIBRARIES)
    set(MAD_INCLUDE_DIRS)
endif()

if(NOT MAD_FOUND)
    set(MAD_NOT_FOUND_MESSAGE "MAD library not found. Set MAD_INCLUDE_DIR and MAD_LIBRARY manually.")
    if(MAD_FIND_REQUIRED)
        message(FATAL_ERROR "${MAD_NOT_FOUND_MESSAGE}")
    endif()
else()
    message("MAD library found")
endif()
