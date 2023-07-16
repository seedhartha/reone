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

if(MSVC)
    find_package(GTest CONFIG REQUIRED)
else()
    find_package(GTest REQUIRED)
endif()

set(TESTS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test)

set(TESTS_HEADERS
    ${TESTS_SOURCE_DIR}/checkutil.h
    ${TESTS_SOURCE_DIR}/fixtures/audio.h
    ${TESTS_SOURCE_DIR}/fixtures/data.h
    ${TESTS_SOURCE_DIR}/fixtures/engine.h
    ${TESTS_SOURCE_DIR}/fixtures/game.h
    ${TESTS_SOURCE_DIR}/fixtures/graphics.h
    ${TESTS_SOURCE_DIR}/fixtures/gui.h
    ${TESTS_SOURCE_DIR}/fixtures/movie.h
    ${TESTS_SOURCE_DIR}/fixtures/resource.h
    ${TESTS_SOURCE_DIR}/fixtures/scene.h
    ${TESTS_SOURCE_DIR}/fixtures/script.h
    ${TESTS_SOURCE_DIR}/fixtures/system.h)

set(TESTS_SOURCES
    ${TESTS_SOURCE_DIR}/audio/format/wavreader.cpp
    ${TESTS_SOURCE_DIR}/graphics/aabb.cpp
    ${TESTS_SOURCE_DIR}/graphics/format/bwmreader.cpp
    ${TESTS_SOURCE_DIR}/graphics/format/mdlmdxreader.cpp
    ${TESTS_SOURCE_DIR}/graphics/format/tgareader.cpp
    ${TESTS_SOURCE_DIR}/graphics/format/tpcreader.cpp
    ${TESTS_SOURCE_DIR}/graphics/format/txireader.cpp
    ${TESTS_SOURCE_DIR}/graphics/walkmesh.cpp
    ${TESTS_SOURCE_DIR}/resource/2das.cpp
    ${TESTS_SOURCE_DIR}/resource/format/2dareader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/2dawriter.cpp
    ${TESTS_SOURCE_DIR}/resource/format/bifreader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/erfreader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/erfwriter.cpp
    ${TESTS_SOURCE_DIR}/resource/format/gffreader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/gffwriter.cpp
    ${TESTS_SOURCE_DIR}/resource/format/keyreader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/rimreader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/rimwriter.cpp
    ${TESTS_SOURCE_DIR}/resource/format/tlkreader.cpp
    ${TESTS_SOURCE_DIR}/resource/format/tlkwriter.cpp
    ${TESTS_SOURCE_DIR}/resource/gffs.cpp
    ${TESTS_SOURCE_DIR}/resource/resources.cpp
    ${TESTS_SOURCE_DIR}/resource/strings.cpp
    ${TESTS_SOURCE_DIR}/scene/model.cpp
    ${TESTS_SOURCE_DIR}/script/execution.cpp
    ${TESTS_SOURCE_DIR}/script/format/ncsreader.cpp
    ${TESTS_SOURCE_DIR}/script/format/ncswriter.cpp
    ${TESTS_SOURCE_DIR}/tools/exprtree.cpp
    ${TESTS_SOURCE_DIR}/tools/exprtreeoptimizer.cpp
    ${TESTS_SOURCE_DIR}/system/binaryreader.cpp
    ${TESTS_SOURCE_DIR}/system/binarywriter.cpp
    ${TESTS_SOURCE_DIR}/system/fileutil.cpp
    ${TESTS_SOURCE_DIR}/system/hexutil.cpp
    ${TESTS_SOURCE_DIR}/system/stream/memoryinput.cpp
    ${TESTS_SOURCE_DIR}/system/stream/memoryoutput.cpp
    ${TESTS_SOURCE_DIR}/system/stream/fileinput.cpp
    ${TESTS_SOURCE_DIR}/system/stream/fileoutput.cpp
    ${TESTS_SOURCE_DIR}/system/stringbuilder.cpp
    ${TESTS_SOURCE_DIR}/system/textreader.cpp
    ${TESTS_SOURCE_DIR}/system/textwriter.cpp
    ${TESTS_SOURCE_DIR}/system/threadpool.cpp
    ${TESTS_SOURCE_DIR}/system/timer.cpp)

add_executable(tests ${TESTS_HEADERS} ${TESTS_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIRS})

target_precompile_headers(tests PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(tests PRIVATE tools GTest::gmock_main)

if(MSVC)
    target_compile_options(tests PRIVATE /bigobj)
endif()

add_test(NAME UnitTests COMMAND tests)
