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

set(GUI_HEADERS
    control.h
    control/button.h
    control/button.h
    control/buttontoggle.h
    control/factory.h
    control/label.h
    control/labelhilight.h
    control/listbox.h
    control/panel.h
    control/plotter.h
    control/progressbar.h
    control/progressbar.h
    control/scrollbar.h
    control/slider.h
    gui.h
    guiinterface.h
    sceneinitializer.h
    textinput.h
    types.h)

set(GUI_SOURCES
    control.cpp
    control/button.cpp
    control/label.cpp
    control/listbox.cpp
    control/plotter.cpp
    gui.cpp
    sceneinitializer.cpp
    textinput.cpp)

add_library(gui STATIC ${GUI_HEADERS} ${GUI_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(gui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(gui PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(gui PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
