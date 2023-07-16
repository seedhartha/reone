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

#include "reone/graphics/textutil.h"

#include "reone/graphics/font.h"

namespace reone {

namespace graphics {

std::vector<std::string> breakText(const std::string &text, Font &font, int maxWidth) {
    std::vector<std::string> result;
    std::string line;
    std::ostringstream tokenBuffer;

    for (char ch : text) {
        switch (ch) {
        case '\n': {
            std::string token(tokenBuffer.str());
            if (!token.empty()) {
                tokenBuffer.str("");
                if (!line.empty()) {
                    line += " ";
                }
                line += token;
            }
            if (!line.empty()) {
                result.push_back(line);
                line = "";
            }
            break;
        }
        case ' ': {
            std::string token(tokenBuffer.str());
            if (!token.empty()) {
                tokenBuffer.str("");

                std::string test(line);
                if (!test.empty()) {
                    test += " ";
                }
                test += token;

                if (font.measure(test) <= maxWidth) {
                    line = std::move(test);
                } else {
                    result.push_back(line);
                    line = std::move(token);
                }
            }
            break;
        }
        default:
            tokenBuffer.put(ch);
            break;
        }
    }

    std::string token(tokenBuffer.str());
    std::string test(line);
    if (!token.empty()) {
        if (!test.empty()) {
            test += " ";
        }
        test += token;
    }
    if (!test.empty()) {
        if (font.measure(test) <= maxWidth) {
            result.push_back(test);
        } else {
            result.push_back(line);
            if (!token.empty()) {
                result.push_back(token);
            }
        }
    }

    return result;
}

} // namespace graphics

} // namespace reone
