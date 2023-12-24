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

bool TextInput::handle(const input::Event &event) {
    switch (event.type) {
    case input::EventType::KeyDown:
        return handleKeyDown(event.key);
    default:
        return false;
    }
}

static inline bool isDigitKey(const input::KeyEvent &event) {
    return event.code >= input::KeyCode::Key0 && event.code <= input::KeyCode::Key9;
}

static inline bool isLetterKey(const input::KeyEvent &event) {
    return event.code >= input::KeyCode::A && event.code <= input::KeyCode::Z;
}

static inline bool isSymbolKey(const input::KeyEvent &event) {
    return event.code == input::KeyCode::Minus ||
           event.code == input::KeyCode::Equals ||
           event.code == input::KeyCode::LeftBracket ||
           event.code == input::KeyCode::RightBracket ||
           event.code == input::KeyCode::Semicolon ||
           event.code == input::KeyCode::Quote ||
           event.code == input::KeyCode::Comma ||
           event.code == input::KeyCode::Period ||
           event.code == input::KeyCode::Slash ||
           event.code == input::KeyCode::Backslash;
}

static inline bool isShiftPressed(const input::KeyEvent &event) {
    return event.mod & input::KeyModifiers::shift;
}

bool TextInput::handleKeyDown(const input::KeyEvent &event) {
    if (!isKeyAllowed(event)) {
        return false;
    }

    bool digit = isDigitKey(event);
    bool letter = isLetterKey(event);
    bool symbol = isSymbolKey(event);
    bool shift = isShiftPressed(event);

    if (event.code == input::KeyCode::Backspace) {
        if (!_text.empty()) {
            _text.resize(_text.size() - 1);
        }
    } else if (event.code == input::KeyCode::Space) {
        _text += static_cast<char>(event.code);
    } else if (digit) {
        if (shift) {
            if (event.code == input::KeyCode::Key1) {
                _text += "!";
            } else if (event.code == input::KeyCode::Key2) {
                _text += "@";
            } else if (event.code == input::KeyCode::Key3) {
                _text += "#";
            } else if (event.code == input::KeyCode::Key4) {
                _text += "$";
            } else if (event.code == input::KeyCode::Key5) {
                _text += "%";
            } else if (event.code == input::KeyCode::Key6) {
                _text += "^";
            } else if (event.code == input::KeyCode::Key7) {
                _text += "&";
            } else if (event.code == input::KeyCode::Key8) {
                _text += "*";
            } else if (event.code == input::KeyCode::Key9) {
                _text += "(";
            } else if (event.code == input::KeyCode::Key0) {
                _text += ")";
            }
        } else {
            _text += static_cast<char>(event.code);
        }
    } else if (letter) {
        _text += shift ? toupper(static_cast<char>(event.code)) : static_cast<char>(event.code);
    } else if (symbol) {
        if (shift) {
            if (event.code == input::KeyCode::Minus) {
                _text += "_";
            } else if (event.code == input::KeyCode::Equals) {
                _text += "+";
            } else if (event.code == input::KeyCode::LeftBracket) {
                _text += "{";
            } else if (event.code == input::KeyCode::RightBracket) {
                _text += "}";
            } else if (event.code == input::KeyCode::Semicolon) {
                _text += ":";
            } else if (event.code == input::KeyCode::Quote) {
                _text += "\"";
            } else if (event.code == input::KeyCode::Comma) {
                _text += "<";
            } else if (event.code == input::KeyCode::Period) {
                _text += ">";
            } else if (event.code == input::KeyCode::Slash) {
                _text += "?";
            } else if (event.code == input::KeyCode::Backslash) {
                _text += "|";
            }
        } else {
            _text += static_cast<char>(event.code);
        }
    }

    return true;
}

bool TextInput::isKeyAllowed(const input::KeyEvent &event) const {
    if (event.code == input::KeyCode::Backspace) {
        return true;
    }
    if (event.code == input::KeyCode::Space) {
        return (_mask & TextInputFlags::whitespace) != 0;
    }
    if (isDigitKey(event)) {
        return (_mask & TextInputFlags::digits) != 0;
    }
    if (isLetterKey(event)) {
        return (_mask & TextInputFlags::letters) != 0;
    }
    if (isSymbolKey(event)) {
        return (_mask & TextInputFlags::symbols) != 0;
    }
    return false;
}

void TextInput::setText(std::string text) {
    _text = std::move(text);
}

} // namespace gui

} // namespace reone
