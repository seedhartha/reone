/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <boost/format.hpp>

#include "src/core/log.h"
#include "src/resources/rimfile.h"
#include "src/resources/util.h"

using namespace std;

using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void RimTool::list(const fs::path &path, const fs::path &keyPath) const {
    RimFile rim;
    rim.load(path);

    for (auto &res : rim.resources()) {
        info(boost::format("%16s\t%4s") % res.resRef % getExtByResType(res.type));
    }
}

void RimTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    RimFile rim;
    rim.load(path);

    auto &resources = rim.resources();
    for (int i = 0; i < resources.size(); ++i) {
        const RimFile::Resource &resource = resources[i];
        info(boost::format("Extracting %16s\t%4s") % resource.resRef % getExtByResType(resource.type));

        fs::path resPath(destPath);
        resPath.append(resource.resRef + "." + getExtByResType(resource.type));

        ByteArray data(rim.getResourceData(i));
        fs::ofstream res(resPath, ios::binary);
        res.write(data.data(), data.size());
    }
}

} // namespace tools

} // namespace reone
