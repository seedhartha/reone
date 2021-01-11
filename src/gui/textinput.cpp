/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "textinput.h"

using namespace std;

namespace reone {

namespace gui {

TextInput::TextInput(int mask) : _mask(mask) {
}

void TextInput::clear() {
    _text.clear();
}

bool TextInput::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        default:
            return false;
    }
}

static inline bool isShiftPressed(const SDL_Keysym &key) {
    return key.mod & KMOD_SHIFT;
}

static inline bool isDigitKey(const SDL_Keysym &key) {
    return key.sym >= SDLK_0 && key.sym <= SDLK_9;
}

static inline bool isLetterKey(const SDL_Keysym &key) {
    return key.sym >= SDLK_a && key.sym <= SDLK_z;
}

bool TextInput::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (!isKeyAllowed(event.keysym)) return false;

    bool shift = isShiftPressed(event.keysym);
    bool digit = isDigitKey(event.keysym);
    bool letter = isLetterKey(event.keysym);
    bool underscore = event.keysym.sym == SDLK_MINUS && shift;

    if (digit || letter || underscore ||
        event.keysym.sym == SDLK_SPACE ||
        event.keysym.sym == SDLK_COMMA ||
        event.keysym.sym == SDLK_PERIOD) {

        if (underscore) {
            _text += SDLK_UNDERSCORE;
        } else if (letter && shift) {
            _text += toupper(event.keysym.sym);
        } else {
            _text += event.keysym.sym;
        }
        return true;

    } else if (event.keysym.sym == SDLK_BACKSPACE) {
        if (!_text.empty()) {
            _text.resize(_text.size() - 1);
        }
        return true;
    }

    return false;
}

bool TextInput::isKeyAllowed(const SDL_Keysym &key) const {
    if (isDigitKey(key) && !(_mask & kTextInputDigits)) return false;
    if (isLetterKey(key) && !(_mask & kTextInputLetters)) return false;
    if (key.sym == SDLK_SPACE && !(_mask & kTextInputWhitespace)) return false;

    bool underscore = key.sym == SDLK_MINUS && isShiftPressed(key);
    if (underscore ||
        key.sym == SDLK_COMMA ||
        key.sym == SDLK_PERIOD) {

        return (_mask & kTextInputPunctuation) != 0;
    }

    return true;
}

const string &TextInput::text() const {
    return _text;
}

void TextInput::setText(string text) {
    _text = move(text);
}

} // namespace gui

} // namespace reone
