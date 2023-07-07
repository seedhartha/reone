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

#include <boost/program_options.hpp>

#include "reone/resource/types.h"
#include "reone/resource/typeutil.h"
#include "reone/system/fileutil.h"

#include "gffschema.h"
#include "guis.h"
#include "routines.h"

using namespace reone;
using namespace reone::resource;

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                                   //
            ("generator", boost::program_options::value<std::string>()->required()) //
            ("destdir", boost::program_options::value<std::string>()->required())   //
            ("k1dir", boost::program_options::value<std::string>()->required())     //
            ("k2dir", boost::program_options::value<std::string>()->required())     //
            ("restype", boost::program_options::value<std::string>());              //

        boost::program_options::variables_map vars;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vars);
        boost::program_options::notify(vars);

        auto &generator = vars["generator"].as<std::string>();

        auto &destDirValue = vars["destdir"].as<std::string>();
        auto destDir = boost::filesystem::path(destDirValue);
        if (!boost::filesystem::exists(destDir) || !boost::filesystem::is_directory(destDir)) {
            throw std::runtime_error("Destination directory does not exist: " + destDir.string());
        }

        auto &k1DirValue = vars["k1dir"].as<std::string>();
        auto k1Dir = boost::filesystem::path(k1DirValue);
        if (!boost::filesystem::exists(k1Dir) || !boost::filesystem::is_directory(k1Dir)) {
            throw std::runtime_error("Directory not found: " + k1Dir.string());
        }

        auto &k2DirValue = vars["k2dir"].as<std::string>();
        auto k2Dir = boost::filesystem::path(k2DirValue);
        if (!boost::filesystem::exists(k2Dir) || !boost::filesystem::is_directory(k2Dir)) {
            throw std::runtime_error("Directory not found: " + k2Dir.string());
        }

        if (generator == "routines") {
            auto k1NssPath = findFileIgnoreCase(k1Dir, "nwscript.nss");
            if (k1NssPath.empty()) {
                throw std::runtime_error("File not found: " + k1NssPath.string());
            }
            auto k2NssPath = findFileIgnoreCase(k1Dir, "nwscript.nss");
            if (k2NssPath.empty()) {
                throw std::runtime_error("File not found: " + k2NssPath.string());
            }
            generateRoutines(k1NssPath, k2NssPath, destDir);

        } else if (generator == "gffschema") {
            if (vars.count("restype") == 0) {
                throw std::runtime_error("Required restype argument not specified");
            }
            auto &restypeValue = vars["restype"].as<std::string>();
            auto restype = getResTypeByExt(restypeValue);
            if (!isGFFCompatibleResType(restype)) {
                throw std::runtime_error("Resource type not GFF compatible: " + restypeValue);
            }
            generateGffSchema(restype, k1Dir, k2Dir, destDir);

        } else if (generator == "guis") {
            generateGuis(k1Dir, k2Dir, destDir);

        } else {
            throw std::runtime_error("Invalid generator argument: " + generator);
        }

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
