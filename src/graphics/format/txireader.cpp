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

#include "txireader.h"

#include "../../common/logutil.h"

using namespace std;

namespace reone {

namespace graphics {

void TxiReader::load(const shared_ptr<istream> &in) {
    if (!in) {
        throw invalid_argument("Invalid TXI input stream");
    }
    char buf[64];
    do {
        in->getline(buf, sizeof(buf));

        string line(buf);
        boost::trim(line);

        vector<string> tokens;
        boost::split(tokens, line, boost::is_space(), boost::token_compress_on);

        processLine(tokens);
    } while (!in->eof());
}

static Texture::ProcedureType parseProcedureType(const string &str) {
    Texture::ProcedureType result = Texture::ProcedureType::Invalid;
    if (str == "cycle") {
        result = Texture::ProcedureType::Cycle;
    } else {
        warn("TXI: invalid procedure type: " + str);
    }
    return result;
}

void TxiReader::processLine(const vector<string> &tokens) {
    string key;
    glm::vec3 vec;

    switch (_state) {
    case State::None:
        key = boost::to_lower_copy(tokens[0]);
        if (key == "envmaptexture") {
            _features.envmapTexture = tokens[1];
        } else if (key == "bumpyshinytexture") {
            _features.bumpyShinyTexture = tokens[1];
        } else if (key == "bumpmaptexture") {
            _features.bumpmapTexture = tokens[1];
        } else if (key == "bumpmapscaling") {
            _features.bumpMapScaling = stof(tokens[1]);
        } else if (key == "blending") {
            _features.blending = parseBlending(tokens[1]);
        } else if (key == "numchars") {
            _features.numChars = stoi(tokens[1]);
        } else if (key == "fontheight") {
            _features.fontHeight = stof(tokens[1]);
        } else if (key == "upperleftcoords") {
            _upperLeftCoordCount = stoi(tokens[1]);
            _features.upperLeftCoords.reserve(_upperLeftCoordCount);
            _state = State::UpperLeftCoords;
        } else if (key == "lowerrightcoords") {
            _lowerRightCoordCount = stoi(tokens[1]);
            _features.lowerRightCoords.reserve(_lowerRightCoordCount);
            _state = State::LowerRightCoords;
        } else if (key == "wateralpha") {
            _features.waterAlpha = stof(tokens[1]);
        } else if (key == "proceduretype") {
            _features.procedureType = parseProcedureType(tokens[1]);
        } else if (key == "numx") {
            _features.numX = stoi(tokens[1]);
        } else if (key == "numy") {
            _features.numY = stoi(tokens[1]);
        } else if (key == "fps") {
            _features.fps = stoi(tokens[1]);
        }
        break;

    case State::UpperLeftCoords:
        vec = glm::vec3(
            stod(tokens[0]),
            stod(tokens[1]),
            stod(tokens[2]));

        _features.upperLeftCoords.push_back(vec);
        if (_features.upperLeftCoords.size() == _upperLeftCoordCount) {
            _state = State::None;
        }
        break;

    case State::LowerRightCoords:
        vec = glm::vec3(
            stod(tokens[0]),
            stod(tokens[1]),
            stod(tokens[2]));

        _features.lowerRightCoords.push_back(vec);
        if (_features.lowerRightCoords.size() == _lowerRightCoordCount) {
            _state = State::None;
        }
        break;
    }
}

Texture::Blending TxiReader::parseBlending(const string &s) const {
    auto result = Texture::Blending::None;
    if (s == "additive") {
        result = Texture::Blending::Additive;
    } else if (s == "punchthrough") {
        result = Texture::Blending::PunchThrough;
    } else {
        debug("TXI: unsupported blending: " + s);
    }
    return result;
}

} // namespace graphics

} // namespace reone
