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

set(AUDIO_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/audio)
set(AUDIO_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/audio)

set(AUDIO_HEADERS
    ${AUDIO_INCLUDE_DIR}/clip.h
    ${AUDIO_INCLUDE_DIR}/context.h
    ${AUDIO_INCLUDE_DIR}/di/module.h
    ${AUDIO_INCLUDE_DIR}/di/services.h
    ${AUDIO_INCLUDE_DIR}/options.h
    ${AUDIO_INCLUDE_DIR}/player.h
    ${AUDIO_INCLUDE_DIR}/source.h
    ${AUDIO_INCLUDE_DIR}/types.h)

set(AUDIO_SOURCES
    ${AUDIO_SOURCE_DIR}/clip.cpp
    ${AUDIO_SOURCE_DIR}/context.cpp
    ${AUDIO_SOURCE_DIR}/di/module.cpp
    ${AUDIO_SOURCE_DIR}/player.cpp
    ${AUDIO_SOURCE_DIR}/source.cpp)

add_library(audio STATIC ${AUDIO_HEADERS} ${AUDIO_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(audio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(audio PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(audio PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(audio PRIVATE ${MAD_LIBRARY})
if(MSVC)
    target_link_libraries(audio PRIVATE OpenAL::OpenAL)
else()
    target_link_libraries(audio PRIVATE ${OpenAL_LIBRARIES})
endif()
