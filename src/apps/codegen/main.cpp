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

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "reone/resource/types.h"
#include "reone/resource/typeutil.h"

#include "gffschema.h"
#include "routines.h"

using namespace reone;
using namespace reone::resource;

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                                   //
            ("generator", boost::program_options::value<std::string>()->required()) //
            ("destdir", boost::program_options::value<std::string>())               //
            ("k1nssfile", boost::program_options::value<std::string>())             //
            ("k2nssfile", boost::program_options::value<std::string>())             //
            ("k1dir", boost::program_options::value<std::string>())                 //
            ("k2dir", boost::program_options::value<std::string>())                 //
            ("restype", boost::program_options::value<std::string>());              //

        boost::program_options::variables_map vars;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vars);
        boost::program_options::notify(vars);

        auto &generator = vars["generator"].as<std::string>();
        if (generator == "routines") {
            if (vars.count("destdir") == 0) {
                throw std::runtime_error("Required destdir argument not specified");
            }
            auto &destdirValue = vars["destdir"].as<std::string>();
            auto destdir = boost::filesystem::path(destdirValue);
            if (!boost::filesystem::exists(destdir) || !boost::filesystem::is_directory(destdir)) {
                throw std::runtime_error("Destination directory does not exist: " + destdir.string());
            }
            if (vars.count("k1nssfile") == 0) {
                throw std::runtime_error("Required k1nssfile argument not specified");
            }
            auto &k1nssfileValue = vars["k1nssfile"].as<std::string>();
            auto k1nssfile = boost::filesystem::path(k1nssfileValue);
            if (!boost::filesystem::exists(k1nssfile) || !boost::filesystem::is_regular_file(k1nssfile)) {
                throw std::runtime_error("File not found: " + k1nssfile.string());
            }
            if (vars.count("k2nssfile") == 0) {
                throw std::runtime_error("Required k2nssfile argument not specified");
            }
            auto &k2nssfileValue = vars["k2nssfile"].as<std::string>();
            auto k2nssfile = boost::filesystem::path(k1nssfileValue);
            if (!boost::filesystem::exists(k2nssfile) || !boost::filesystem::is_regular_file(k2nssfile)) {
                throw std::runtime_error("File not found: " + k2nssfile.string());
            }
            generateRoutines(k1nssfile, k2nssfile, destdir);

        } else if (generator == "gffschema") {
            if (vars.count("destdir") == 0) {
                throw std::runtime_error("Required destdir argument not specified");
            }
            auto &destdirValue = vars["destdir"].as<std::string>();
            auto destdir = boost::filesystem::path(destdirValue);
            if (!boost::filesystem::exists(destdir) || !boost::filesystem::is_directory(destdir)) {
                throw std::runtime_error("Destination directory does not exist: " + destdir.string());
            }
            if (vars.count("k1dir") == 0) {
                throw std::runtime_error("Required k1dir argument not specified");
            }
            auto &k1dirValue = vars["k1dir"].as<std::string>();
            auto k1dir = boost::filesystem::path(k1dirValue);
            if (!boost::filesystem::exists(k1dir) || !boost::filesystem::is_directory(k1dir)) {
                throw std::runtime_error("Directory not found: " + k1dir.string());
            }
            if (vars.count("k2dir") == 0) {
                throw std::runtime_error("Required k2dir argument not specified");
            }
            auto &k2dirValue = vars["k2dir"].as<std::string>();
            auto k2dir = boost::filesystem::path(k2dirValue);
            if (!boost::filesystem::exists(k2dir) || !boost::filesystem::is_directory(k2dir)) {
                throw std::runtime_error("Directory not found: " + k2dir.string());
            }
            if (vars.count("restype") == 0) {
                throw std::runtime_error("Required restype argument not specified");
            }
            auto &restypeValue = vars["restype"].as<std::string>();
            auto restype = getResTypeByExt(restypeValue);
            if (!isGFFCompatibleResType(restype)) {
                throw std::runtime_error("Resource type not GFF compatible: " + restypeValue);
            }
            generateGffSchema(restype, k1dir, k2dir, destdir);

        } else {
            throw std::runtime_error("Invalid generator argument: " + generator);
        }

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
