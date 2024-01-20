/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/resource/types.h"
#include "reone/resource/typeutil.h"
#include "reone/system/checkutil.h"

#include "guis.h"
#include "models.h"
#include "routines.h"
#include "templates.h"

using namespace reone;
using namespace reone::resource;

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                             //
            ("job", boost::program_options::value<std::string>()->required()) //
            ("k1dir", boost::program_options::value<std::string>())           //
            ("k2dir", boost::program_options::value<std::string>())           //
            ("destdir", boost::program_options::value<std::string>())         //
            ("restype", boost::program_options::value<std::string>());        //

        boost::program_options::variables_map vars;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vars);
        boost::program_options::notify(vars);

        auto &job = vars["job"].as<std::string>();

        std::optional<std::filesystem::path> k1Dir;
        if (vars.count("k1dir") > 0) {
            auto &k1DirValue = vars["k1dir"].as<std::string>();
            k1Dir = std::filesystem::path(k1DirValue);
            if (!std::filesystem::exists(*k1Dir) || !std::filesystem::is_directory(*k1Dir)) {
                throw std::runtime_error("Directory not found: " + k1Dir->string());
            }
        }

        std::optional<std::filesystem::path> k2Dir;
        if (vars.count("k2dir") > 0) {
            auto &k2DirValue = vars["k2dir"].as<std::string>();
            k2Dir = std::filesystem::path(k2DirValue);
            if (!std::filesystem::exists(*k2Dir) || !std::filesystem::is_directory(*k2Dir)) {
                throw std::runtime_error("Directory not found: " + k2Dir->string());
            }
        }

        std::optional<ResType> restype;
        if (vars.count("restype") > 0) {
            auto &restypeValue = vars["restype"].as<std::string>();
            restype = getResTypeByExt(restypeValue);
            if (!isGFFCompatibleResType(*restype)) {
                throw std::runtime_error("Resource type not GFF compatible: " + restypeValue);
            }
        }

        std::optional<std::filesystem::path> destDir;
        if (vars.count("destdir") > 0) {
            auto &destDirValue = vars["destdir"].as<std::string>();
            destDir = std::filesystem::path(destDirValue);
            if (!std::filesystem::exists(*destDir) || !std::filesystem::is_directory(*destDir)) {
                throw std::runtime_error("Destination directory does not exist: " + destDir->string());
            }
        }

        if (job == "routines") {
            checkThat(static_cast<bool>(k1Dir), "Missing required k1dir argument");
            checkThat(static_cast<bool>(k2Dir), "Missing required k2dir argument");
            checkThat(static_cast<bool>(destDir), "Missing required destdir argument");
            generateRoutines(*k1Dir, *k2Dir, *destDir);

        } else if (job == "templates") {
            checkThat(static_cast<bool>(k1Dir), "Missing required k1dir argument");
            checkThat(static_cast<bool>(k2Dir), "Missing required k2dir argument");
            checkThat(static_cast<bool>(destDir), "Missing required destdir argument");
            checkThat(static_cast<bool>(restype), "Missing required restype argument");
            generateTemplates(*restype, *k1Dir, *k2Dir, *destDir);

        } else if (job == "guis") {
            checkThat(static_cast<bool>(k1Dir), "Missing required k1dir argument");
            checkThat(static_cast<bool>(k2Dir), "Missing required k2dir argument");
            checkThat(static_cast<bool>(destDir), "Missing required destdir argument");
            generateGuis(*k1Dir, *k2Dir, *destDir);

        } else if (job == "models") {
            checkThat(static_cast<bool>(k1Dir), "Missing required k1dir argument");
            checkThat(static_cast<bool>(k2Dir), "Missing required k2dir argument");
            analyzeModels(*k1Dir, *k2Dir);

        } else {
            throw std::runtime_error("Invalid job argument: " + job);
        }

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
