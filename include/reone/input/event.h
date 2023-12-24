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

enum class KeyState {
    Released,
    Pressed
};

enum class Scancode : uint16_t {
    None = 0,

    A = 4,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Key1 = 30,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    Key0,

    Enter = 40,
    Esc,
    Backspace,
    Tab,
    Space,
    Minus,
    Equal,
    LeftBrace,
    RightBrace,
    Backslash,
    HashTilde,
    Semicolon,
    Apostrophe,
    Grave,
    Comma,
    Dot,
    Slash,
    CapsLock,

    F1 = 58,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    SysReq = 70,
    ScrollLock,
    Pause,
    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,
    Right,
    Left,
    Down,
    Up,

    LeftControl = 224,
    LeftShift,
    LeftAlt,
    LeftGUI,
    RightControl,
    RightShift,
    RightAlt,
    RightGUI
};

struct KeyModifiers {
    static constexpr int leftControl = 1 << 0;
    static constexpr int leftShift = 1 << 1;
    static constexpr int leftAlt = 1 << 2;
    static constexpr int leftGUI = 1 << 3;
    static constexpr int rightControl = 1 << 4;
    static constexpr int rightShift = 1 << 5;
    static constexpr int rightAlt = 1 << 6;
    static constexpr int rightGUI = 1 << 7;

    static constexpr int control = leftControl | rightControl;
    static constexpr int shift = leftShift | rightShift;
    static constexpr int alt = leftAlt | rightAlt;
    static constexpr int gui = leftGUI | rightGUI;
};

struct KeyEvent {
    KeyState state;
    Scancode scancode;
    uint16_t mod;
    bool repeat;

    KeyEvent(KeyState state,
             Scancode scancode,
             uint16_t mod,
             bool repeat) :
        state(state),
        scancode(scancode),
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
    Left,
    Middle,
    Right,
    X1,
    X2
};

enum class MouseState {
    Released,
    Pressed
};

struct MouseButtonEvent {
    MouseButton button;
    MouseState state;
    uint8_t clicks;
    int32_t x;
    int32_t y;

    MouseButtonEvent(MouseButton button,
                     MouseState state,
                     uint8_t clicks,
                     int32_t x,
                     int32_t y) :
        button(button),
        state(state),
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
