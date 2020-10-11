/*
 * Copyright © 2020 Vsevolod Kremianskii
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>

#include "SDL_events.h"

namespace reone {

namespace gui {

enum TextInputFlags {
    kTextInputDigits = 1,
    kTextInputLetters = 2,
    kTextInputWhitespace = 4,
    kTextInputPunctuation = 8,
    kTextInputConsole = kTextInputDigits | kTextInputLetters | kTextInputWhitespace | kTextInputPunctuation
};

class TextInput {
public:
    TextInput(int mask);

    void clear();
    bool handle(const SDL_Event &event);

    const std::string &text() const;

private:
    int _mask { 0 };
    std::string _text;

    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool isKeyAllowed(const SDL_Keysym &key) const;
};

} // namespace gui

} // namespace reone
