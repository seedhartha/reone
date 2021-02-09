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

void FileTool::list(const fs::path &path, const fs::path &keyPath) const {
    throwNotImplemented();
}

void FileTool::throwNotImplemented() const {
    throw logic_error("Not implemented");
}

void FileTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    throwNotImplemented();
}

void FileTool::toJson(const fs::path &path, const fs::path &destPath) const {
    throwNotImplemented();
}

void FileTool::toTga(const fs::path &path, const fs::path &destPath) const {
    throwNotImplemented();
}

void FileTool::to2DA(const fs::path &path, const fs::path &destPath) const {
    throwNotImplemented();
}

void FileTool::toGFF(const fs::path &path, const fs::path &destPath) const {
    throwNotImplemented();
}

unique_ptr<FileTool> getFileToolByPath(GameID gameId, const fs::path &path) {
    if (fs::is_directory(path)) {
        throw invalid_argument("path must not point to a directory");
    }
    unique_ptr<FileTool> result;

    string ext(path.extension().string());
    if (ext == ".key") {
        result = make_unique<KeyTool>();
    } else if (ext == ".bif") {
        result = make_unique<BifTool>();
    } else if (ext == ".erf" || ext == ".mod" || ext == ".sav") {
        result = make_unique<ErfTool>();
    } else if (ext == ".rim") {
        result = make_unique<RimTool>();
    } else if (ext == ".2da") {
        result = make_unique<TwoDaTool>();
    } else if (ext == ".tlk") {
        result =  make_unique<TlkTool>();
    } else if (ext == ".tpc") {
        result = make_unique<TpcTool>();
    } else if (ext == ".json") {
        result = make_unique<JsonTool>();
    } else {
        result = make_unique<GffTool>();
    }

    return move(result);
}

} // namespace tools

} // namespace reone
