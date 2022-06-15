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
    audio/format/mp3reader.cpp
    audio/format/wavreader.cpp
    common/binaryreader.cpp
    common/binarywriter.cpp
    common/collectionutil.cpp
    common/hexutil.cpp
    common/pathutil.cpp
    common/stream/bytearrayinput.cpp
    common/stream/bytearrayoutput.cpp
    common/stream/fileinput.cpp
    common/stream/fileoutput.cpp
    common/stringbuilder.cpp
    common/textwriter.cpp
    common/unicodeutil.cpp
    game/action/movetoobject.cpp
    game/astar.cpp
    game/game.cpp
    game/object.cpp
    game/object/creature.cpp
    game/object/item.cpp
    game/object/placeable.cpp
    game/selectioncontroller.cpp
    graphics/format/bwmreader.cpp
    graphics/format/mdlreader.cpp
    graphics/format/tgareader.cpp
    graphics/format/tpcreader.cpp
    graphics/format/txireader.cpp
    main.cpp
    resource/2das.cpp
    resource/format/2dareader.cpp
    resource/format/2dawriter.cpp
    resource/format/bifreader.cpp
    resource/format/erfreader.cpp
    resource/format/erfwriter.cpp
    resource/format/gffreader.cpp
    resource/format/gffwriter.cpp
    resource/format/keyreader.cpp
    resource/format/rimreader.cpp
    resource/format/rimwriter.cpp
    resource/format/tlkreader.cpp
    resource/format/tlkwriter.cpp
    resource/gffs.cpp
    resource/resources.cpp
    resource/strings.cpp
    script/execution.cpp
    script/format/ncsreader.cpp
    script/format/ncswriter.cpp
    toolslib/expressiontree.cpp)

add_executable(reone-tests ${TEST_HEADERS} ${TEST_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_compile_definitions(reone-tests PRIVATE BOOST_TEST_DYN_LINK)
target_precompile_headers(reone-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone-tests PRIVATE toolslib ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME UnitTests COMMAND reone-tests)
