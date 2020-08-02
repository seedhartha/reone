#include "streamutil.h"

#include <boost/iostreams/stream.hpp>

namespace io = boost::iostreams;

namespace reone {

std::unique_ptr<std::istream> wrap(const ByteArray &arr) {
    io::array_source source(arr.data(), arr.size());
    return std::make_unique<io::stream<io::array_source>>(source);
}

} // namespace reone
