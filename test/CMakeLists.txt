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

set(TEST_HEADERS
    checkutil.h
    fixtures/functional.h
    fixtures/game.h
    fixtures/scene.h)

set(TEST_SOURCES
    audio/mp3reader.cpp
    audio/wavreader.cpp
    common/binaryreader.cpp
    common/binarywriter.cpp
    common/bytearrayinputstream.cpp
    common/bytearrayoutputstream.cpp
    common/collectionutil.cpp
    common/fileinputstream.cpp
    common/fileoutputstream.cpp
    common/hexutil.cpp
    common/pathutil.cpp
    common/stringbuilder.cpp
    common/textwriter.cpp
    common/unicodeutil.cpp
    game/action/movetoobject.cpp
    game/astar.cpp
    game/game.cpp
    game/object.cpp
    game/selectioncontroller.cpp
    graphics/bwmreader.cpp
    graphics/mdlreader.cpp
    graphics/tgareader.cpp
    graphics/tpcreader.cpp
    graphics/txireader.cpp
    main.cpp
    resource/2dareader.cpp
    resource/2das.cpp
    resource/2dawriter.cpp
    resource/bifreader.cpp
    resource/erfreader.cpp
    resource/erfwriter.cpp
    resource/gffreader.cpp
    resource/gffs.cpp
    resource/gffwriter.cpp
    resource/keyreader.cpp
    resource/resources.cpp
    resource/rimreader.cpp
    resource/rimwriter.cpp
    resource/strings.cpp
    resource/tlkreader.cpp
    resource/tlkwriter.cpp
    script/execution.cpp
    script/ncsreader.cpp
    script/ncswriter.cpp
    toolslib/expressiontree.cpp)

add_executable(reone-tests ${TEST_HEADERS} ${TEST_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_compile_definitions(reone-tests PRIVATE BOOST_TEST_DYN_LINK)
target_precompile_headers(reone-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone-tests PRIVATE toolslib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME UnitTests COMMAND reone-tests)
