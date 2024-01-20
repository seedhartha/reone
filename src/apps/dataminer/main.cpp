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

#include "guis.h"
#include "models.h"
#include "routines.h"
#include "templates.h"

using namespace reone;
using namespace reone::resource;

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                                 //
            ("tool", boost::program_options::value<std::string>()->required())    //
            ("destdir", boost::program_options::value<std::string>()->required()) //
            ("k1dir", boost::program_options::value<std::string>()->required())   //
            ("k2dir", boost::program_options::value<std::string>()->required())   //
            ("restype", boost::program_options::value<std::string>());            //

        boost::program_options::variables_map vars;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vars);
        boost::program_options::notify(vars);

        auto &tool = vars["tool"].as<std::string>();

        auto &destDirValue = vars["destdir"].as<std::string>();
        auto destDir = std::filesystem::path(destDirValue);
        if (!std::filesystem::exists(destDir) || !std::filesystem::is_directory(destDir)) {
            throw std::runtime_error("Destination directory does not exist: " + destDir.string());
        }

        auto &k1DirValue = vars["k1dir"].as<std::string>();
        auto k1Dir = std::filesystem::path(k1DirValue);
        if (!std::filesystem::exists(k1Dir) || !std::filesystem::is_directory(k1Dir)) {
            throw std::runtime_error("Directory not found: " + k1Dir.string());
        }

        auto &k2DirValue = vars["k2dir"].as<std::string>();
        auto k2Dir = std::filesystem::path(k2DirValue);
        if (!std::filesystem::exists(k2Dir) || !std::filesystem::is_directory(k2Dir)) {
            throw std::runtime_error("Directory not found: " + k2Dir.string());
        }

        if (tool == "routines") {
            generateRoutines(k1Dir, k2Dir, destDir);

        } else if (tool == "templates") {
            if (vars.count("restype") == 0) {
                throw std::runtime_error("Required restype argument not specified");
            }
            auto &restypeValue = vars["restype"].as<std::string>();
            auto restype = getResTypeByExt(restypeValue);
            if (!isGFFCompatibleResType(restype)) {
                throw std::runtime_error("Resource type not GFF compatible: " + restypeValue);
            }
            generateTemplates(restype, k1Dir, k2Dir, destDir);

        } else if (tool == "guis") {
            generateGuis(k1Dir, k2Dir, destDir);

        } else if (tool == "models") {
            analyzeModels(k1Dir, k2Dir);

        } else {
            throw std::runtime_error("Invalid tool argument: " + tool);
        }

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
