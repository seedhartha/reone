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

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(TEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test)

set(TEST_HEADERS
    ${TEST_SOURCE_DIR}/checkutil.h
    ${TEST_SOURCE_DIR}/fixtures/audio.h
    ${TEST_SOURCE_DIR}/fixtures/game.h
    ${TEST_SOURCE_DIR}/fixtures/graphics.h
    ${TEST_SOURCE_DIR}/fixtures/resource.h
    ${TEST_SOURCE_DIR}/fixtures/scene.h
    ${TEST_SOURCE_DIR}/fixtures/script.h)

set(TEST_SOURCES
    ${TEST_SOURCE_DIR}/audio/format/wavreader.cpp
    ${TEST_SOURCE_DIR}/system/binaryreader.cpp
    ${TEST_SOURCE_DIR}/system/binarywriter.cpp
    ${TEST_SOURCE_DIR}/system/collectionutil.cpp
    ${TEST_SOURCE_DIR}/system/hexutil.cpp
    ${TEST_SOURCE_DIR}/system/pathutil.cpp
    ${TEST_SOURCE_DIR}/system/stream/bytearrayinput.cpp
    ${TEST_SOURCE_DIR}/system/stream/bytearrayoutput.cpp
    ${TEST_SOURCE_DIR}/system/stream/fileinput.cpp
    ${TEST_SOURCE_DIR}/system/stream/fileoutput.cpp
    ${TEST_SOURCE_DIR}/system/stringbuilder.cpp
    ${TEST_SOURCE_DIR}/system/textwriter.cpp
    ${TEST_SOURCE_DIR}/game/game.cpp
    ${TEST_SOURCE_DIR}/graphics/aabb.cpp
    ${TEST_SOURCE_DIR}/graphics/format/bwmreader.cpp
    ${TEST_SOURCE_DIR}/graphics/format/mdlreader.cpp
    ${TEST_SOURCE_DIR}/graphics/format/tgareader.cpp
    ${TEST_SOURCE_DIR}/graphics/format/tpcreader.cpp
    ${TEST_SOURCE_DIR}/graphics/format/txireader.cpp
    ${TEST_SOURCE_DIR}/graphics/walkmesh.cpp
    ${TEST_SOURCE_DIR}/main.cpp
    ${TEST_SOURCE_DIR}/resource/2das.cpp
    ${TEST_SOURCE_DIR}/resource/format/2dareader.cpp
    ${TEST_SOURCE_DIR}/resource/format/2dawriter.cpp
    ${TEST_SOURCE_DIR}/resource/format/bifreader.cpp
    ${TEST_SOURCE_DIR}/resource/format/erfreader.cpp
    ${TEST_SOURCE_DIR}/resource/format/erfwriter.cpp
    ${TEST_SOURCE_DIR}/resource/format/gffreader.cpp
    ${TEST_SOURCE_DIR}/resource/format/gffwriter.cpp
    ${TEST_SOURCE_DIR}/resource/format/keyreader.cpp
    ${TEST_SOURCE_DIR}/resource/format/rimreader.cpp
    ${TEST_SOURCE_DIR}/resource/format/rimwriter.cpp
    ${TEST_SOURCE_DIR}/resource/format/tlkreader.cpp
    ${TEST_SOURCE_DIR}/resource/format/tlkwriter.cpp
    ${TEST_SOURCE_DIR}/resource/gffs.cpp
    ${TEST_SOURCE_DIR}/resource/resources.cpp
    ${TEST_SOURCE_DIR}/resource/strings.cpp
    ${TEST_SOURCE_DIR}/scene/model.cpp
    ${TEST_SOURCE_DIR}/script/execution.cpp
    ${TEST_SOURCE_DIR}/script/format/ncsreader.cpp
    ${TEST_SOURCE_DIR}/script/format/ncswriter.cpp
    ${TEST_SOURCE_DIR}/tools/expressiontree.cpp)

add_executable(reone-tests ${TEST_HEADERS} ${TEST_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/bin)
target_compile_definitions(reone-tests PRIVATE BOOST_TEST_DYN_LINK)
target_precompile_headers(reone-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone-tests PRIVATE toolslib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME UnitTests COMMAND reone-tests)
