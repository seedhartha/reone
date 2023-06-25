/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/gui/textinput.h"

namespace reone {

namespace gui {

TextInput::TextInput(int mask) :
    _mask(mask) {
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

static inline bool isDigitKey(const SDL_Keysym &key) {
    return key.sym >= SDLK_0 && key.sym <= SDLK_9;
}

static inline bool isLetterKey(const SDL_Keysym &key) {
    return key.sym >= SDLK_a && key.sym <= SDLK_z;
}

static inline bool isSymbolKey(const SDL_Keysym &key) {
    return key.sym == SDLK_MINUS ||
           key.sym == SDLK_EQUALS ||
           key.sym == SDLK_LEFTBRACKET ||
           key.sym == SDLK_RIGHTBRACKET ||
           key.sym == SDLK_SEMICOLON ||
           key.sym == SDLK_QUOTE ||
           key.sym == SDLK_COMMA ||
           key.sym == SDLK_PERIOD ||
           key.sym == SDLK_SLASH ||
           key.sym == SDLK_BACKSLASH;
}

static inline bool isShiftPressed(const SDL_Keysym &key) {
    return key.mod & KMOD_SHIFT;
}

bool TextInput::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (!isKeyAllowed(event.keysym)) {
        return false;
    }

    bool digit = isDigitKey(event.keysym);
    bool letter = isLetterKey(event.keysym);
    bool symbol = isSymbolKey(event.keysym);
    bool shift = isShiftPressed(event.keysym);

    if (event.keysym.sym == SDLK_BACKSPACE) {
        if (!_text.empty()) {
            _text.resize(_text.size() - 1);
        }
    } else if (event.keysym.sym == SDLK_SPACE) {
        _text += event.keysym.sym;
    } else if (digit) {
        if (shift) {
            if (event.keysym.sym == SDLK_1) {
                _text += "!";
            } else if (event.keysym.sym == SDLK_2) {
                _text += "@";
            } else if (event.keysym.sym == SDLK_3) {
                _text += "#";
            } else if (event.keysym.sym == SDLK_4) {
                _text += "$";
            } else if (event.keysym.sym == SDLK_5) {
                _text += "%";
            } else if (event.keysym.sym == SDLK_6) {
                _text += "^";
            } else if (event.keysym.sym == SDLK_7) {
                _text += "&";
            } else if (event.keysym.sym == SDLK_8) {
                _text += "*";
            } else if (event.keysym.sym == SDLK_9) {
                _text += "(";
            } else if (event.keysym.sym == SDLK_0) {
                _text += ")";
            }
        } else {
            _text += event.keysym.sym;
        }
    } else if (letter) {
        _text += shift ? toupper(event.keysym.sym) : event.keysym.sym;
    } else if (symbol) {
        if (shift) {
            if (event.keysym.sym == SDLK_MINUS) {
                _text += "_";
            } else if (event.keysym.sym == SDLK_EQUALS) {
                _text += "+";
            } else if (event.keysym.sym == SDLK_LEFTBRACKET) {
                _text += "{";
            } else if (event.keysym.sym == SDLK_RIGHTBRACKET) {
                _text += "}";
            } else if (event.keysym.sym == SDLK_SEMICOLON) {
                _text += ":";
            } else if (event.keysym.sym == SDLK_QUOTE) {
                _text += "\"";
            } else if (event.keysym.sym == SDLK_COMMA) {
                _text += "<";
            } else if (event.keysym.sym == SDLK_PERIOD) {
                _text += ">";
            } else if (event.keysym.sym == SDLK_SLASH) {
                _text += "?";
            } else if (event.keysym.sym == SDLK_BACKSLASH) {
                _text += "|";
            }
        } else {
            _text += event.keysym.sym;
        }
    }

    return true;
}

bool TextInput::isKeyAllowed(const SDL_Keysym &key) const {
    if (key.sym == SDLK_BACKSPACE) {
        return true;
    }
    if (key.sym == SDLK_SPACE) {
        return (_mask & TextInputFlags::whitespace) != 0;
    }
    if (isDigitKey(key)) {
        return (_mask & TextInputFlags::digits) != 0;
    }
    if (isLetterKey(key)) {
        return (_mask & TextInputFlags::letters) != 0;
    }
    if (isSymbolKey(key)) {
        return (_mask & TextInputFlags::symbols) != 0;
    }
    return false;
}

void TextInput::setText(std::string text) {
    _text = std::move(text);
}

} // namespace gui

} // namespace reone
