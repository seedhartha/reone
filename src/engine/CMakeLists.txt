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

set(REONE_HEADERS
    di/module/audio.h
    di/module/graphics.h
    di/module/kotor.h
    di/module/resource.h
    di/module/scene.h
    di/module/script.h
    di/services.h
    engine.h
    gameprobe.h
    optionsparser.h
    types.h)

set(REONE_SOURCES
    di/module/audio.cpp
    di/module/graphics.cpp
    di/module/kotor.cpp
    di/module/resource.cpp
    di/module/scene.cpp
    di/module/script.cpp
    di/services.cpp
    engine.cpp
    gameprobe.cpp
    main.cpp
    optionsparser.cpp)

if(WIN32)
    list(APPEND REONE_SOURCES ${CMAKE_SOURCE_DIR}/res/reone.rc)
endif()

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_precompile_headers(reone PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone PRIVATE kotor ${Boost_PROGRAM_OPTIONS_LIBRARY})

list(APPEND InstallTargets reone)
