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

set(GUI_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/gui)
set(GUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/libs/gui)

set(GUI_HEADERS
    ${GUI_INCLUDE_DIR}/control.h
    ${GUI_INCLUDE_DIR}/control/button.h
    ${GUI_INCLUDE_DIR}/control/imagebutton.h
    ${GUI_INCLUDE_DIR}/control/label.h
    ${GUI_INCLUDE_DIR}/control/listbox.h
    ${GUI_INCLUDE_DIR}/control/panel.h
    ${GUI_INCLUDE_DIR}/control/progressbar.h
    ${GUI_INCLUDE_DIR}/control/scrollbar.h
    ${GUI_INCLUDE_DIR}/control/slider.h
    ${GUI_INCLUDE_DIR}/control/togglebutton.h
    ${GUI_INCLUDE_DIR}/di/module.h
    ${GUI_INCLUDE_DIR}/di/services.h
    ${GUI_INCLUDE_DIR}/gui.h
    ${GUI_INCLUDE_DIR}/guis.h
    ${GUI_INCLUDE_DIR}/sceneinitializer.h
    ${GUI_INCLUDE_DIR}/textinput.h
    ${GUI_INCLUDE_DIR}/types.h)

set(GUI_SOURCES
    ${GUI_SOURCE_DIR}/control.cpp
    ${GUI_SOURCE_DIR}/control/imagebutton.cpp
    ${GUI_SOURCE_DIR}/control/listbox.cpp
    ${GUI_SOURCE_DIR}/control/progressbar.cpp
    ${GUI_SOURCE_DIR}/control/scrollbar.cpp
    ${GUI_SOURCE_DIR}/control/togglebutton.cpp
    ${GUI_SOURCE_DIR}/di/module.cpp
    ${GUI_SOURCE_DIR}/gui.cpp
    ${GUI_SOURCE_DIR}/guis.cpp
    ${GUI_SOURCE_DIR}/sceneinitializer.cpp
    ${GUI_SOURCE_DIR}/textinput.cpp)

add_library(gui STATIC ${GUI_HEADERS} ${GUI_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(gui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(gui PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(gui PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
