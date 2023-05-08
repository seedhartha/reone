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

cmake_minimum_required(VERSION 3.16)
project(reone)

set(CMAKE_CXX_STANDARD 17)
set(CLANG_FORMAT_PATH ${CMAKE_SOURCE_DIR}/.clang-format)

# Options

option(BUILD_TOOLS "build tools executable" OFF)
option(BUILD_LAUNCHER "build launcher executable" OFF)
option(BUILD_TESTS "build tests" OFF)

option(ENABLE_MOVIE "enable movie playback" ON)
option(ENABLE_ASAN "enable address sanitizer" OFF)

# END Options

# Dependencies

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(Boost REQUIRED COMPONENTS filesystem program_options regex system OPTIONAL_COMPONENTS unit_test_framework)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)
find_package(wxWidgets COMPONENTS core base)

if(MSVC)
    find_package(OpenAL CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    find_package(Threads REQUIRED)
endif()

set(REONE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/extern/glm ${CMAKE_SOURCE_DIR}/extern/tinyxml2 ${REONE_INCLUDE_DIR})

if(ENABLE_MOVIE)
    if(MSVC)
        find_package(FFMPEG REQUIRED)
    else()
        pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswresample libswscale)
    endif()
    include_directories(${FFMPEG_INCLUDE_DIRS})
endif()

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# END Dependencies

# Compile Options

if(ENABLE_ASAN AND MSVC)
    add_compile_options("/fsanitize=address")
endif()

# END Compile Options

# Engine libraries

include(cmake/common.cmake) # common static library
include(cmake/resource.cmake) # resource static library
include(cmake/graphics.cmake) # graphics static library
include(cmake/audio.cmake) # audio static library
include(cmake/gui.cmake) # gui static library
include(cmake/scene.cmake) # scene static library
include(cmake/movie.cmake) # movie static library
include(cmake/script.cmake) # script static library
include(cmake/game.cmake) # game static library

# END Engine libraries

include(cmake/engine.cmake) # reone executable

if(BUILD_TOOLS)
    include(cmake/tinyxml2.cmake) # tinyxml2 static library
    include(cmake/toolslib.cmake) # tools static library
    include(cmake/tools.cmake) # reone-tools executable
endif()

if(BUILD_LAUNCHER)
    include(cmake/launcher.cmake) # reone-launcher executable
endif()

if(BUILD_TESTS)
    enable_testing()
    include(cmake/test.cmake) # reone-tests executable
endif()

## Installation

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

install(TARGETS ${InstallTargets} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS common resource graphics audio scene movie script gui game DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(BUILD_TOOLS)
    install(TARGETS toolslib DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

## END Installation
