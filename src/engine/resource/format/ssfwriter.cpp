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

#include "ssfwriter.h"

#include "../../common/streamwriter.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

void SsfWriter::save(const fs::path &path) {
    auto stream = make_shared<fs::ofstream>(path);
    StreamWriter writer(stream);

    writer.putString("SSF V1.1");
    writer.putUint32(12); // offset to entries

    for (auto val : _soundSet) {
        writer.putUint32(val);
    }
}

} // namespace resource

} // namespace reone
