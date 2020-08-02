#pragma once

#include <istream>
#include <memory>
#include <vector>

#include "../render/types.h"

namespace reone {

namespace resources {

class TxiFile {
public:
    void load(const std::shared_ptr<std::istream> &in);
    const render::TextureFeatures &features() const;

private:
    enum class State {
        None,
        UpperLeftCoords,
        LowerRightCoords
    };

    render::TextureFeatures _features;
    State _state { State::None };
    int _upperLeftCoordCount { 0 };
    int _lowerRightCoordCount { 0 };

    void processLine(const std::vector<std::string> &tokens);
    render::TextureBlending parseBlending(const std::string &s) const;
};

} // namespace resources

} // namespace reone
