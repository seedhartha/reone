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

#include "reone/resource/format/erfwriter.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

using namespace reone;
using namespace reone::resource;

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                                              //
            ("srcdir", boost::program_options::value<std::filesystem::path>()->required())   //
            ("destdir", boost::program_options::value<std::filesystem::path>()->required()); //

        boost::program_options::positional_options_description positionalDesc;
        positionalDesc.add("srcdir", 1);
        positionalDesc.add("destdir", 1);

        auto options = boost::program_options::command_line_parser(argc, argv)
                           .options(description)
                           .positional(positionalDesc)
                           .run();

        boost::program_options::variables_map vars;
        boost::program_options::store(options, vars);
        boost::program_options::notify(vars);

        auto &srcdir = vars["srcdir"].as<std::filesystem::path>();
        if (!std::filesystem::exists(srcdir) || !std::filesystem::is_directory(srcdir)) {
            throw std::runtime_error("Source directory does not exist: " + srcdir.string());
        }

        auto &destdir = vars["destdir"].as<std::filesystem::path>();
        if (!std::filesystem::exists(destdir) || !std::filesystem::is_directory(destdir)) {
            throw std::runtime_error("Destination directory does not exist: " + destdir.string());
        }

        auto writer = ErfWriter();

        for (auto &entry : std::filesystem::directory_iterator(srcdir)) {
            if (!std::filesystem::is_regular_file(entry.status())) {
                continue;
            }
            if (entry.path().extension() != ".glsl") {
                continue;
            }
            auto glslStream = FileInputStream(entry.path());
            glslStream.seek(0, SeekOrigin::End);
            auto filesize = glslStream.position();
            glslStream.seek(0, SeekOrigin::Begin);
            auto glslBytes = ByteBuffer();
            glslBytes.resize(filesize);
            glslStream.read(&glslBytes[0], filesize);

            auto resRef = entry.path().filename();
            resRef.replace_extension();

            ErfWriter::Resource resource;
            resource.resRef = resRef.string();
            resource.resType = ResourceType::Glsl;
            resource.data = std::move(glslBytes);
            writer.add(std::move(resource));
        }

        auto erfPath = destdir;
        erfPath.append("shaderpack.erf");

        auto erfStream = FileOutputStream(erfPath);
        writer.save(ErfWriter::FileType::ERF, erfStream);

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
