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

cmake_minimum_required(VERSION 3.16)
project(reone)

set(CMAKE_CXX_STANDARD 17)
set(CLANG_FORMAT_PATH ${CMAKE_SOURCE_DIR}/.clang-format)

# Options

option(BUILD_TESTS "build tests" ON)
option(BUILD_LAUNCHER "build launcher application" ON)
option(BUILD_TOOLKIT "build toolkit application" ON)
option(BUILD_DATAMINER "build dataminer application" ON)

option(ENABLE_MOVIE "enable movie playback" ON)
option(ENABLE_ASAN "enable address sanitizer" OFF)

# END Options

# Dependencies

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS program_options exception)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)

if(MSVC)
    find_package(OpenAL CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    find_package(Threads REQUIRED)
endif()

if(ENABLE_MOVIE)
    if(MSVC)
        find_package(FFMPEG REQUIRED)
    else()
        pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswresample libswscale)
    endif()
    include_directories(${FFMPEG_INCLUDE_DIRS})
endif()

if(BUILD_LAUNCHER OR BUILD_TOOLKIT)
    find_package(wxWidgets REQUIRED core base adv stc gl aui)
    include(${wxWidgets_USE_FILE})
endif()

set(REONE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/extern/glm ${REONE_INCLUDE_DIR})

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# END Dependencies

# Compile options

if(ENABLE_ASAN)
    if (MSVC)
        add_compile_options("/fsanitize=address")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endif()

# END Compile options

# Libraries

add_subdirectory(src/libs/system) # system static library
add_subdirectory(src/libs/resource) # resource static library
add_subdirectory(src/libs/graphics) # graphics static library
add_subdirectory(src/libs/audio) # audio static library
add_subdirectory(src/libs/input) # input static library
add_subdirectory(src/libs/gui) # gui static library
add_subdirectory(src/libs/scene) # scene static library
add_subdirectory(src/libs/movie) # movie static library
add_subdirectory(src/libs/script) # script static library
add_subdirectory(src/libs/game) # game static library
add_subdirectory(src/libs/tools) # tools static library

# END Libraries

# Applications

add_subdirectory(src/apps/shaderpack) # shaderpack application
add_subdirectory(src/apps/engine) # engine application

if(BUILD_LAUNCHER)
    add_subdirectory(src/apps/launcher) # launcher application
endif()

if(BUILD_TOOLKIT)
    add_subdirectory(src/apps/toolkit) # toolkit application
endif()

if(BUILD_DATAMINER)
    add_subdirectory(src/apps/dataminer) # dataminer application
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test) # tests executable
endif()

# END Applications

# Installation

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

install(TARGETS ${InstallTargets} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS system resource graphics audio scene movie script gui game tools DESTINATION ${CMAKE_INSTALL_LIBDIR})

# END Installation
