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

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(TEST_HEADERS
    checkutil.h)

set(TEST_SOURCES
    2dareader.cpp
    2dawriter.cpp
    bifreader.cpp
    erfreader.cpp
    erfwriter.cpp
    gffreader.cpp
    gffwriter.cpp
    keyreader.cpp
    main.cpp
    ncsreader.cpp
    ncswriter.cpp
    rimreader.cpp
    rimwriter.cpp
    streamreader.cpp
    streamwriter.cpp
    tlkreader.cpp
    tlkwriter.cpp
    unicode.cpp)

add_executable(reone-tests ${TEST_HEADERS} ${TEST_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(reone-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_compile_definitions(reone-tests PRIVATE BOOST_TEST_DYN_LINK)
target_precompile_headers(reone-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(reone-tests PRIVATE game ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME UnitTests COMMAND reone-tests)
