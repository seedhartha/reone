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

#include "tools.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void Tool::list(const fs::path &path, const fs::path &keyPath) const {
    throwNotImplemented();
}

void Tool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    throwNotImplemented();
}

void Tool::convert(const fs::path &path, const fs::path &destPath) const {
    throwNotImplemented();
}

void Tool::throwNotImplemented() const {
    throw logic_error("Not implemented");
}

unique_ptr<Tool> getToolByPath(GameVersion version, const fs::path &path) {
    string ext(path.extension().string());
    if (ext == ".key") {
        return make_unique<KeyTool>();
    } else if (ext == ".bif") {
        return make_unique<BifTool>();
    } else if (ext == ".erf" || ext == ".mod" || ext == ".sav") {
        return make_unique<ErfTool>();
    } else if (ext == ".rim") {
        return make_unique<RimTool>();
    } else if (ext == ".2da") {
        return make_unique<TwoDaTool>();
    } else if (ext == ".tlk") {
        return make_unique<TlkTool>();
    } else {
        return make_unique<GffTool>();
    }
}

} // namespace tools

} // namespace reone
