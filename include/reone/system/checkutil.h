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

#include "exception/validation.h"

namespace reone {

inline void checkThat(bool condition, const std::string &message) {
    if (!condition) {
        throw ValidationException(message);
    }
}

template <class T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline void checkEqual(const std::string &param, const T &actual, const T &expected) {
    if (actual != expected) {
        throw ValidationException(str(boost::format("%s expected to be equal to %d, was %d") % param % expected % actual));
    }
}

template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
inline void checkEqual(const std::string &param, const T &actual, const T &expected) {
    if (actual != expected) {
        throw ValidationException(str(boost::format("%s expected to be equal to %d, was %d") %
                                      param %
                                      static_cast<int>(expected) %
                                      static_cast<int>(actual)));
    }
}

inline void checkEqual(const std::string &param, const std::string &actual, const std::string &expected) {
    if (actual != expected) {
        throw ValidationException(str(boost::format("%s expected to be equal to '%s', was '%s'") % param % expected % actual));
    }
}

template <class T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline void checkNotEqual(const std::string &param, const T &actual, const T &expected) {
    if (actual == expected) {
        throw ValidationException(str(boost::format("%s expected not to be equal to %d, but was") % param % expected));
    }
}

template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
inline void checkNotEqual(const std::string &param, const T &actual, const T &expected) {
    if (actual == expected) {
        throw ValidationException(str(boost::format("%s expected not to be equal to %d, but was") %
                                      param %
                                      static_cast<int>(expected)));
    }
}

inline void checkNotEqual(const std::string &param, const std::string &actual, const std::string &expected) {
    if (actual == expected) {
        throw ValidationException(str(boost::format("%s expected not to be equal to '%s', but was") % param % expected));
    }
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
inline void checkLess(const std::string &param, const T &lhs, const T &rhs) {
    if (lhs >= rhs) {
        throw ValidationException(str(boost::format("%1% expected to be less than %2%, was %3%") % param % rhs % lhs));
    }
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
inline void checkLessOrEqual(const std::string &param, const T &lhs, const T &rhs) {
    if (lhs > rhs) {
        throw ValidationException(str(boost::format("%1% expected to be less or equal to %2%, was %3%") % param % rhs % lhs));
    }
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
inline void checkGreater(const std::string &param, const T &lhs, const T &rhs) {
    if (lhs <= rhs) {
        throw ValidationException(str(boost::format("%1% expected to be greater than %2%, was %3%") % param % rhs % lhs));
    }
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
inline void checkGreaterOrEqual(const std::string &param, const T &lhs, const T &rhs) {
    if (lhs < rhs) {
        throw ValidationException(str(boost::format("%1% expected to be greater or equal to %2%, was %3%") % param % rhs % lhs));
    }
}

} // namespace reone
