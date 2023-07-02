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

namespace reone {

extern void generateRoutines(const boost::filesystem::path &k1nssfile,
                             const boost::filesystem::path &k2nssfile,
                             const boost::filesystem::path &destDir);

}

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                                               //
            ("k1nssfile", boost::program_options::value<boost::filesystem::path>()->required()) //
            ("k2nssfile", boost::program_options::value<boost::filesystem::path>()->required()) //
            ("destdir", boost::program_options::value<boost::filesystem::path>()->required());  //

        boost::program_options::positional_options_description positionalDesc;
        positionalDesc.add("k1nssfile", 1);
        positionalDesc.add("k2nssfile", 1);
        positionalDesc.add("destdir", 1);

        auto options = boost::program_options::command_line_parser(argc, argv)
                           .options(description)
                           .positional(positionalDesc)
                           .run();

        boost::program_options::variables_map vars;
        boost::program_options::store(options, vars);
        boost::program_options::notify(vars);

        auto &k1nssfile = vars["k1nssfile"].as<boost::filesystem::path>();
        if (!boost::filesystem::exists(k1nssfile) || !boost::filesystem::is_regular_file(k1nssfile)) {
            throw std::runtime_error("File not found: " + k1nssfile.string());
        }

        auto &k2nssfile = vars["k2nssfile"].as<boost::filesystem::path>();
        if (!boost::filesystem::exists(k2nssfile) || !boost::filesystem::is_regular_file(k1nssfile)) {
            throw std::runtime_error("File not found: " + k2nssfile.string());
        }

        auto &destdir = vars["destdir"].as<boost::filesystem::path>();
        if (!boost::filesystem::exists(destdir) || !boost::filesystem::is_directory(destdir)) {
            throw std::runtime_error("Destination directory does not exist: " + destdir.string());
        }

        reone::generateRoutines(k1nssfile, k2nssfile, destdir);

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
