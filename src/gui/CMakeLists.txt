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

set(GUI_HEADERS
    control/button.h
    control/control.h
    control/imagebutton.h
    control/label.h
    control/listbox.h
    control/panel.h
    control/progressbar.h
    control/scrollbar.h
    control/slider.h
    control/togglebutton.h
    gui.h
    sceneinitializer.h
    textinput.h
    types.h)

set(GUI_SOURCES
    control/control.cpp
    control/imagebutton.cpp
    control/listbox.cpp
    control/progressbar.cpp
    control/scrollbar.cpp
    control/togglebutton.cpp
    gui.cpp
    sceneinitializer.cpp
    textinput.cpp)

add_library(gui STATIC ${GUI_HEADERS} ${GUI_SOURCES})
set_target_properties(gui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(gui PRIVATE ${CMAKE_SOURCE_DIR}/src/engine/pch.h)
