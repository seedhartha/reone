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

#include "reone/system/unicodeutil.h"

#include "reone/system/checkutil.h"

namespace reone {

std::vector<uint32_t> codePointsFromUTF8(const std::string &s) {
    std::vector<uint32_t> result;
    auto bytes = reinterpret_cast<const uint8_t *>(&s[0]);
    for (size_t i = 0; i < s.length();) {
        uint32_t cp;
        auto b1 = bytes[i];
        if ((b1 & 0x80) == 0) {
            // 1-byte code point
            cp = b1 & 0x7f;
            ++i;
        } else if ((b1 & 0xe0) == 0xc0) {
            // 2-byte code point
            checkGreater("length", s.length(), i + 1);
            auto b2 = bytes[i + 1];
            checkThat((b2 & 0xc0) == 0x80, "second byte must begin with bits 10");
            cp = (b1 & 0x1f) << 6;
            cp |= b2 & 0x3f;
            i += 2;
        } else if ((b1 & 0xf0) == 0xe0) {
            // 3-byte code point
            checkGreater("length", s.length(), i + 2);
            auto b2 = bytes[i + 1];
            checkThat((b2 & 0xc0) == 0x80, "second byte must begin with bits 10");
            auto b3 = bytes[i + 2];
            checkThat((b3 & 0xc0) == 0x80, "third byte must begin with bits 10");
            cp = (b1 & 0x0f) << 12;
            cp |= (b2 & 0x3f) << 6;
            cp |= b3 & 0x3f;
            i += 3;
        } else if ((b1 & 0xf8) == 0xf0) {
            // 4-byte code point
            checkGreater("length", s.length(), i + 3);
            auto b2 = bytes[i + 1];
            checkThat((b2 & 0xc0) == 0x80, "second byte must begin with bits 10");
            auto b3 = bytes[i + 2];
            checkThat((b3 & 0xc0) == 0x80, "third byte must begin with bits 10");
            auto b4 = bytes[i + 3];
            checkThat((b4 & 0xc0) == 0x80, "fourth byte must begin with bits 10");
            cp = (b1 & 0x07) << 18;
            cp |= (b2 & 0x3f) << 12;
            cp |= (b3 & 0x3f) << 6;
            cp |= b4 & 0x3f;
            i += 4;
        } else {
            throw std::invalid_argument(str(boost::format("Unexpected start of UTF-8 code point at %d: %02x") % i % b1));
        }
        result.emplace_back(cp);
    }
    return result;
}

std::string utf8FromCodePoints(const std::vector<uint32_t> codePoints) {
    std::ostringstream result;
    for (size_t i = 0; i < codePoints.size(); ++i) {
        const auto &cp = codePoints[i];
        if (cp <= 0x7f) {
            result.put(cp);
        } else if (cp <= 0x7ff) {
            auto b1 = ((cp & 0x7c0) >> 6) | 0xc0;
            auto b2 = (cp & 0x3f) | 0x80;
            result.put(b1);
            result.put(b2);
        } else if (cp <= 0xffff) {
            auto b1 = ((cp & 0xf000) >> 12) | 0xe0;
            auto b2 = ((cp & 0xfc0) >> 6) | 0x80;
            auto b3 = (cp & 0x3f) | 0x80;
            result.put(b1);
            result.put(b2);
            result.put(b3);
        } else if (cp <= 0x10ffff) {
            auto b1 = ((cp & 0x1c0000) >> 18) | 0xf0;
            auto b2 = ((cp & 0x3f000) >> 12) | 0x80;
            auto b3 = ((cp & 0xfc0) >> 6) | 0x80;
            auto b4 = (cp & 0x3f) | 0x80;
            result.put(b1);
            result.put(b2);
            result.put(b3);
            result.put(b4);
        } else {
            throw std::invalid_argument(str(boost::format("Code point out of range at %d: %08x") % i % cp));
        }
    }
    return result.str();
}

std::vector<uint32_t> codePointsFromUTF16(const std::u16string &s) {
    std::vector<uint32_t> result;
    auto units = reinterpret_cast<const uint16_t *>(&s[0]);
    for (size_t i = 0; i < s.length();) {
        auto &unit1 = units[i];
        if ((unit1 & 0xfc00) == 0xd800) {
            // surrogate pair
            checkGreater("length", s.length(), i + 1);
            auto &unit2 = units[i + 1];
            checkThat((unit2 & 0xfc00) == 0xdc00, "second code unit must begin with bits 110111");
            auto codePoint = (unit1 & 0x3ff) << 10;
            codePoint |= unit2 & 0x3ff;
            codePoint += 0x10000;
            result.emplace_back(codePoint);
            i += 2;
        } else if (unit1 <= 0xd7ff || (0xe000 <= unit1 && unit1 <= 0xffff)) {
            // single code
            result.emplace_back(unit1);
            ++i;
        } else {
            throw std::invalid_argument(str(boost::format("Unsupported UTF-16 code unit at %d: %04x") % i % unit1));
        }
    }
    return std::move(result);
}

std::u16string utf16FromCodePoints(const std::vector<uint32_t> &codePoints) {
    std::basic_ostringstream<char16_t> result;
    for (size_t i = 0; i < codePoints.size(); ++i) {
        auto &cp = codePoints[i];
        if (cp <= 0xd7ff || (0xe000 <= cp && cp <= 0xffff)) {
            result.put(cp);
        } else if (cp >= 0x10000 && cp <= 0x10ffff) {
            auto cp2 = cp - 0x10000;
            auto unit1 = (cp2 >> 10) | 0xd800;
            auto unit2 = (cp2 & 0x3ff) | 0xdc00;
            result.put(unit1);
            result.put(unit2);
        } else {
            throw std::invalid_argument(str(boost::format("Unsupported UTF-16 code point at %d: %08x") % i % cp));
        }
    }
    return result.str();
}

bool isCodePointAlpha(uint32_t codePoint) {
    if (codePoint >= 0x41 && codePoint <= 0x5a) {
        // Capital Latin
        return true;
    }
    if (codePoint >= 0x61 && codePoint <= 0x7a) {
        // Small Latin
        return true;
    }
    if ((codePoint >= 0x0410 && codePoint <= 0x044f) ||
        codePoint == 0x0401 || codePoint == 0x0451) {
        // Cyrillic
        return true;
    }
    return false;
}

uint32_t codePointToLower(uint32_t codePoint) {
    if (codePoint >= 0x41 && codePoint <= 0x5a) {
        // Latin
        return codePoint + 0x20;
    }
    if (codePoint >= 0x410 && codePoint <= 0x42f) {
        // Cyrillic
        return codePoint + 0x20;
    }
    if (codePoint == 0x0401) {
        // Cyrillic Io
        return 0x0451;
    }
    return codePoint;
}

uint32_t codePointToUpper(uint32_t codePoint) {
    if (codePoint >= 0x61 && codePoint <= 0x7a) {
        // Latin
        return codePoint - 0x20;
    }
    if (codePoint >= 0x430 && codePoint <= 0x44f) {
        // Cyrillic
        return codePoint - 0x20;
    }
    if (codePoint == 0x0451) {
        // Cyrillic Io
        return 0x0401;
    }
    return codePoint;
}

} // namespace reone
