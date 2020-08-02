#include "visfile.h"

#include <stdexcept>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace reone {

namespace resources {

void VisFile::load(const std::shared_ptr<std::istream> &in) {
    if (!in) {
        throw std::invalid_argument("Invalid input stream");
    }
    char buf[32];
    do {
        in->getline(buf, sizeof(buf));

        std::string line(buf);
        boost::trim(line);

        if (line.empty()) continue;

        processLine(line);
    } while (!in->eof());
}

void VisFile::processLine(const std::string &line) {
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_space(), boost::token_compress_on);

    if (tokens.size() == 2) {
        _roomFrom = boost::to_lower_copy(tokens[0]);
    } else {
        std::string room(boost::to_lower_copy(tokens[0]));
        _visibility.insert(std::make_pair(_roomFrom, std::move(room)));
    }
}

const Visibility &VisFile::visibility() const {
    return _visibility;
}

} // namespace resources

} // namespace reone
