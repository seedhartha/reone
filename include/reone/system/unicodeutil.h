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

std::vector<uint32_t> codePointsFromUTF8(const std::string &s);
std::string utf8FromCodePoints(const std::vector<uint32_t> codePoints);

std::vector<uint32_t> codePointsFromUTF16(const std::u16string &s);
std::u16string utf16FromCodePoints(const std::vector<uint32_t> &codePoints);

bool isCodePointAlpha(uint32_t codePoint);

uint32_t codePointToLower(uint32_t codePoint);
uint32_t codePointToUpper(uint32_t codePoint);

} // namespace reone
