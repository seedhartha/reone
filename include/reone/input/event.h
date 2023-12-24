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

#pragma once

namespace reone {

namespace input {

enum class KeyCode {
    Unknown = 0,

    Return = '\r',
    Escape = '\x1B',
    Backspace = '\b',
    Tab = '\t',
    Space = ' ',
    Exclaim = '!',
    DoubleQuote = '"',
    Hash = '#',
    Percent = '%',
    Dollar = '$',
    Ampersand = '&',
    Quote = '\'',
    LeftParenthesis = '(',
    RightParenthesis = ')',
    Asterisk = '*',
    Plus = '+',
    Comma = ',',
    Minus = '-',
    Period = '.',
    Slash = '/',
    Key0 = '0',
    Key1 = '1',
    Key2 = '2',
    Key3 = '3',
    Key4 = '4',
    Key5 = '5',
    Key6 = '6',
    Key7 = '7',
    Key8 = '8',
    Key9 = '9',
    Colon = ':',
    Semicolon = ';',
    Less = '<',
    Equals = '=',
    Greater = '>',
    Question = '?',
    At = '@',

    LeftBracket = '[',
    Backslash = '\\',
    RightBracket = ']',
    Caret = '^',
    Underscore = '_',
    Backquote = '`',
    A = 'a',
    B = 'b',
    C = 'c',
    D = 'd',
    E = 'e',
    F = 'f',
    G = 'g',
    H = 'h',
    I = 'i',
    J = 'j',
    K = 'k',
    L = 'l',
    M = 'm',
    N = 'n',
    O = 'o',
    P = 'p',
    Q = 'q',
    R = 'r',
    S = 's',
    T = 't',
    U = 'u',
    V = 'v',
    W = 'w',
    X = 'x',
    Y = 'y',
    Z = 'z',

    CapsLock = 57 | (1 << 30),

    F1 = 58 | (1 << 30),
    F2 = 59 | (1 << 30),
    F3 = 60 | (1 << 30),
    F4 = 61 | (1 << 30),
    F5 = 62 | (1 << 30),
    F6 = 63 | (1 << 30),
    F7 = 64 | (1 << 30),
    F8 = 65 | (1 << 30),
    F9 = 66 | (1 << 30),
    F10 = 67 | (1 << 30),
    F11 = 68 | (1 << 30),
    F12 = 69 | (1 << 30),

    PrintScreen = 70 | (1 << 30),
    ScrollLock = 71 | (1 << 30),
    Pause = 72 | (1 << 30),
    Insert = 73 | (1 << 30),
    Home = 74 | (1 << 30),
    PageUp = 75 | (1 << 30),
    Delete = '\x7F',
    End = 77 | (1 << 30),
    PageDown = 78 | (1 << 30),
    Right = 79 | (1 << 30),
    Left = 80 | (1 << 30),
    Down = 81 | (1 << 30),
    Up = 82 | (1 << 30),

    LeftControl = 224 | (1 << 30),
    LeftShift = 225 | (1 << 30),
    LeftAlt = 226 | (1 << 30),
    LeftGUI = 227 | (1 << 30),
    RightControl = 228 | (1 << 30),
    RightShift = 229 | (1 << 30),
    RightAlt = 230 | (1 << 30),
    RightGUI = 231 | (1 << 30)
};

struct KeyModifiers {
    static constexpr int leftShift = 1 << 0;
    static constexpr int rightShift = 1 << 1;
    static constexpr int leftControl = 1 << 6;
    static constexpr int rightControl = 1 << 7;
    static constexpr int leftAlt = 1 << 8;
    static constexpr int rightAlt = 1 << 9;
    static constexpr int leftGUI = 1 << 10;
    static constexpr int rightGUI = 1 << 11;
    static constexpr int capsLock = 1 << 13;

    static constexpr int control = leftControl | rightControl;
    static constexpr int shift = leftShift | rightShift;
    static constexpr int alt = leftAlt | rightAlt;
    static constexpr int gui = leftGUI | rightGUI;
};

struct KeyEvent {
    bool pressed;
    KeyCode code;
    uint16_t mod;
    bool repeat;

    KeyEvent(bool pressed,
             KeyCode code,
             uint16_t mod,
             bool repeat) :
        pressed(pressed),
        code(code),
        mod(mod),
        repeat(repeat) {
    }
};

struct MouseMotionEvent {
    int32_t x;
    int32_t y;
    int32_t xrel;
    int32_t yrel;

    MouseMotionEvent(
        int32_t x,
        int32_t y,
        int32_t xrel,
        int32_t yrel) :
        x(x),
        y(y),
        xrel(xrel),
        yrel(yrel) {
    }
};

enum class MouseButton {
    Left = 1,
    Middle,
    Right,
    X1,
    X2
};

struct MouseButtonEvent {
    MouseButton button;
    bool pressed;
    uint8_t clicks;
    int32_t x;
    int32_t y;

    MouseButtonEvent(MouseButton button,
                     bool pressed,
                     uint8_t clicks,
                     int32_t x,
                     int32_t y) :
        button(button),
        pressed(pressed),
        clicks(clicks),
        x(x),
        y(y) {
    }
};

enum class MouseWheelDirection {
    Normal,
    Flipped
};

struct MouseWheelEvent {
    int32_t x;
    int32_t y;
    MouseWheelDirection direction;

    MouseWheelEvent(int32_t x,
                    int32_t y,
                    MouseWheelDirection direction) :
        x(x),
        y(y),
        direction(direction) {
    }
};

enum class EventType {
    KeyDown,
    KeyUp,
    MouseMotion,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel
};

struct Event {
    EventType type;
    union {
        KeyEvent key;
        MouseMotionEvent motion;
        MouseButtonEvent button;
        MouseWheelEvent wheel;
    };

    Event(EventType type) :
        type(type) {
    }

    inline static Event newKeyDown(KeyEvent key) {
        Event event {EventType::KeyDown};
        event.key = std::move(key);
        return event;
    }

    inline static Event newKeyUp(KeyEvent key) {
        Event event {EventType::KeyUp};
        event.key = std::move(key);
        return event;
    }

    inline static Event newMouseMotion(MouseMotionEvent motion) {
        Event event {EventType::MouseMotion};
        event.motion = std::move(motion);
        return event;
    }

    inline static Event newMouseButtonDown(MouseButtonEvent button) {
        Event event {EventType::MouseButtonDown};
        event.button = std::move(button);
        return event;
    }

    inline static Event newMouseButtonUp(MouseButtonEvent button) {
        Event event {EventType::MouseButtonUp};
        event.button = std::move(button);
        return event;
    }

    inline static Event newMouseWheel(MouseWheelEvent wheel) {
        Event event {EventType::MouseWheel};
        event.wheel = std::move(wheel);
        return event;
    }
};

} // namespace input

} // namespace reone
