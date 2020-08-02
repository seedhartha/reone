#include "txifile.h"

#include <vector>

#include <boost/algorithm/string.hpp>

using namespace reone::render;

namespace reone {

namespace resources {

void TxiFile::load(const std::shared_ptr<std::istream> &in) {
    if (!in) {
        throw std::invalid_argument("TXI: invalid input stream");
    }

    char buf[64];

    do {
        in->getline(buf, sizeof(buf));

        std::string line(buf);
        boost::trim(line);

        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_space(), boost::token_compress_on);

        processLine(tokens);
    } while (!in->eof());
}

void TxiFile::processLine(const std::vector<std::string> &tokens) {
    std::string key;
    glm::vec3 vec;

    switch (_state) {
        case State::None:
            key = boost::to_lower_copy(tokens[0]);
            if (key == "envmaptexture") {
                _features.envMapTexture = tokens[1];
            } else if (key == "bumpyshinytexture") {
                _features.bumpyShinyTexture = tokens[1];
            } else if (key == "bumpmaptexture") {
                _features.bumpMapTexture = tokens[1];
            } else if (key == "blending") {
                _features.blending = parseBlending(tokens[1]);
            } else if (key == "numchars") {
                _features.numChars = std::stoi(tokens[1]);
            } else if (key == "fontheight") {
                _features.fontHeight = std::stof(tokens[1]);
            } else if (key == "upperleftcoords") {
                _upperLeftCoordCount = std::stoi(tokens[1]);
                _features.upperLeftCoords.reserve(_upperLeftCoordCount);
                _state = State::UpperLeftCoords;
            } else if (key == "lowerrightcoords") {
                _lowerRightCoordCount = std::stoi(tokens[1]);
                _features.lowerRightCoords.reserve(_lowerRightCoordCount);
                _state = State::LowerRightCoords;
            }
            break;

        case State::UpperLeftCoords:
            vec = glm::vec3(
                std::stod(tokens[0]),
                std::stod(tokens[1]),
                std::stod(tokens[2]));

            _features.upperLeftCoords.push_back(vec);
            if (_features.upperLeftCoords.size() == _upperLeftCoordCount) {
                _state = State::None;
            }
            break;

        case State::LowerRightCoords:
            vec = glm::vec3(
                std::stod(tokens[0]),
                std::stod(tokens[1]),
                std::stod(tokens[2]));

            _features.lowerRightCoords.push_back(vec);
            if (_features.lowerRightCoords.size() == _lowerRightCoordCount) {
                _state = State::None;
            }
            break;
    }
}

TextureBlending TxiFile::parseBlending(const std::string &s) const {
    if (s == "additive") {
        return TextureBlending::Additive;
    }

    return TextureBlending::None;
}

const TextureFeatures &TxiFile::features() const {
    return _features;
}

} // namespace resources

} // namespace reone
