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

if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})

    set(LAUNCHER_HEADERS
        app.h
        frame.h)

    set(LAUNCHER_SOURCES
        app.cpp
        frame.cpp)

    if(WIN32)
        list(APPEND LAUNCHER_SOURCES ${CMAKE_SOURCE_DIR}/res/reone.rc)
        add_executable(reone-launcher WIN32 ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES} ${CLANG_FORMAT_PATH})
    else()
        add_executable(reone-launcher ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES})
    endif()

    set_target_properties(reone-launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    target_precompile_headers(reone-launcher PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
    target_link_libraries(reone-launcher ${wxWidgets_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

    list(APPEND InstallTargets reone-launcher)
else()
    message(WARNING "wxWidgets not found - launcher cannot be built")
endif()
