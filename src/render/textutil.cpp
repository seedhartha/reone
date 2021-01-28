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

#include "textutil.h"

#include <sstream>

#include <boost/algorithm/string.hpp>

#include "font.h"

using namespace std;

namespace reone {

namespace render {

vector<string> breakText(const string &text, Font &font, int maxWidth) {
    vector<string> result;
    string line;
    ostringstream tokenBuffer;

    for (char ch : text) {
        switch (ch) {
            case '\n': {
                string token(tokenBuffer.str());
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
                string token(tokenBuffer.str());
                if (!token.empty()) {
                    tokenBuffer.str("");

                    string test(line);
                    if (!test.empty()) {
                        test += " ";
                    }
                    test += token;

                    if (font.measure(test) <= maxWidth) {
                        line = move(test);
                    } else {
                        result.push_back(line);
                        line = move(token);
                    }
                }
                break;
            }
            default:
                tokenBuffer.put(ch);
                break;
        }
    }

    string token(tokenBuffer.str());
    if (!token.empty()) {
        if (!line.empty()) {
            line += " ";
        }
        line += token;
    }
    if (!line.empty()) {
        result.push_back(move(line));
    }

    return move(result);
}

} // namespace render

} // namespace reone
