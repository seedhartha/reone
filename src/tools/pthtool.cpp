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

#include <map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/vec3.hpp"

#include "../engine/common/streamutil.h"
#include "../engine/resource/format/gffreader.h"
#include "../engine/resource/format/gffwriter.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

struct PathPoint {
    string name;
    glm::vec3 position { 0.0f };
    vector<int> connections;
};

void PthTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToPTH) {
        toPTH(target, destPath);
    } else if (operation == Operation::ToASCII) {
        toASCII(target, destPath);
    }
}

void PthTool::toPTH(const fs::path &path, const fs::path &destPath) {
    vector<PathPoint> points;
    map<string, int> pointIdxByName;

    // Read ASCII PTH

    fs::ifstream in(path);
    string line;

    // First pass: read points only
    while (getline(in, line).good()) {
        boost::trim(line);

        vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);

        if (tokens.size() == 5ll) {
            // This line contains a point definition
            PathPoint point;
            point.name = tokens[0];
            point.position = glm::vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
            pointIdxByName.insert(make_pair(point.name, static_cast<int>(points.size())));
            points.push_back(move(point));
        }
    }

    // Second pass: read connections
    in.clear();
    in.seekg(0, ios::beg);
    int pointIdx = -1;
    while (getline(in, line).good()) {
        boost::trim(line);

        vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);

        if (tokens.size() == 5ll) {
            // This line contains a point definition
            ++pointIdx;
        } else if (tokens.size() == 1ll) {
            // This line contains a connection
            auto maybePair = pointIdxByName.find(tokens[0]);
            if (maybePair != pointIdxByName.end()) {
                int destinationIdx = maybePair->second;
                points[pointIdx].connections.push_back(destinationIdx);
            }
        }
    }


    // Write binary PTH

    string filename(path.filename().string());
    int lastDotIdx = filename.find_last_of('.');
    if (lastDotIdx != -1) {
        filename = filename.substr(0, lastDotIdx);
    }

    fs::path pthPath(destPath);
    pthPath.append(filename + "-bin.pth");

    int numConnections = 0;
    GffStruct::Field pathPoints(GffStruct::FieldType::List, "Path_Points");
    for (auto &point : points) {
        GffStruct::Field conections(GffStruct::FieldType::Dword, "Conections");
        conections.uintValue = static_cast<int>(point.connections.size());

        GffStruct::Field firstConection(GffStruct::FieldType::Dword, "First_Conection");
        firstConection.uintValue = numConnections;

        GffStruct::Field xField(GffStruct::FieldType::Float, "X");
        xField.floatValue = point.position.x;

        GffStruct::Field yField(GffStruct::FieldType::Float, "Y");
        yField.floatValue = point.position.y;

        auto pointStruct = make_shared<GffStruct>(2);
        pointStruct->add(move(conections));
        pointStruct->add(move(firstConection));
        pointStruct->add(move(xField));
        pointStruct->add(move(yField));
        pathPoints.children.push_back(move(pointStruct));

        numConnections += static_cast<int>(point.connections.size());
    }

    GffStruct::Field pathConections(GffStruct::FieldType::List, "Path_Conections");
    for (auto &point : points) {
        for (auto &connection : point.connections) {
            GffStruct::Field destination(GffStruct::FieldType::Dword, "Destination");
            destination.uintValue = connection;

            auto conectionStruct = make_shared<GffStruct>(3);
            conectionStruct->add(move(destination));
            pathConections.children.push_back(move(conectionStruct));
        }
    }

    auto gffRoot = make_shared<GffStruct>(0xffffffff);
    gffRoot->add(move(pathPoints));
    gffRoot->add(move(pathConections));

    GffWriter writer(ResourceType::Pth, gffRoot);
    writer.save(pthPath);
}

static string getPointName(int index) {
    return str(boost::format("PathPoint%03d") % index);
}

void PthTool::toASCII(const fs::path &path, const fs::path &destPath) {
    // Read binary PTH

    GffReader reader;
    reader.load(path);
    shared_ptr<GffStruct> root(reader.root());

    // Write ASCII PTH

    string filename(path.filename().string());
    int lastDotIdx = filename.find_last_of('.');
    if (lastDotIdx != -1) {
        filename = filename.substr(0, lastDotIdx);
    }

    fs::path asciiPath(destPath);
    asciiPath.append(filename + "-ascii.pth");

    auto ascii = make_shared<fs::ofstream>(asciiPath);
    int pointIdx = 0;
    StreamWriter writer(ascii);
    for (auto &point : root->getList("Path_Points")) {
        string name(getPointName(pointIdx++));
        int conections = point->getInt("Conections");
        int firstConection = point->getInt("First_Conection");
        float x = point->getFloat("X");
        float y = point->getFloat("Y");
        writer.putString(str(boost::format("%s %f %f %f %d\n") % name % x % y % 0.0f % conections));
        for (int i = 0; i < conections; ++i) {
            shared_ptr<GffStruct> conection(root->getList("Path_Conections")[firstConection + i]);
            int destination = conection->getInt("Destination");
            string destName(getPointName(destination));
            writer.putString(str(boost::format("  %s\n") % destName));
        }
    }
}

bool PthTool::supports(Operation operation, const fs::path &target) const {
    return
        !fs::is_directory(target) &&
        target.extension() == ".pth" &&
        (operation == Operation::ToPTH || operation == Operation::ToASCII);
}

} // namespace tools

} // namespace reone
