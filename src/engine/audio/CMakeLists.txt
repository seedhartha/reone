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

set(AUDIO_HEADERS
    files.h
    format/mp3reader.h
    format/wavreader.h
    options.h
    player.h
    soundhandle.h
    soundinstance.h
    stream.h
    types.h)

set(AUDIO_SOURCES
    files.cpp
    format/mp3reader.cpp
    format/wavreader.cpp
    player.cpp
    soundhandle.cpp
    soundinstance.cpp
    stream.cpp)

add_library(libaudio STATIC ${AUDIO_HEADERS} ${AUDIO_SOURCES})
set_target_properties(libaudio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libaudio PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)
target_link_libraries(libaudio PUBLIC ${MAD_LIBRARY})
if(MSVC)
    target_link_libraries(libaudio PUBLIC OpenAL::OpenAL)
else()
    target_link_libraries(libaudio PUBLIC ${OpenAL_LIBRARIES})
endif()
