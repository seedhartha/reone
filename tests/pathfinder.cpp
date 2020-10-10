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

#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "../src/game/pathfinder.h"
#include "../src/resource/pthfile.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;

int main(int argc, char **argv) {
    vector<PthFile::Point> points = {
        { 1.0f, 1.0f, { 1, 2 } },
        { 1.0f, 2.0f, { 0, 2 } },
        { 2.0f, 2.0f, { 1, 3, 4 } },
        { 3.0f, 2.0f, { 2, 4 } },
        { 3.0f, 3.0f, { 2, 3 } }
    };
    unordered_map<int, float> pointZ;
    for (int i = 0; i < points.size(); ++i) {
        pointZ.insert(make_pair(i, 0.0f));
    }
    Pathfinder pathfinder;
    pathfinder.load(points, pointZ);
    auto path = pathfinder.findPath(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(3.0f, 3.0f, 0.0f));

    if (path.size() != 3 ||
        path[0] != glm::vec3(1.0f, 1.0f, 0.0f) ||
        path[1] != glm::vec3(2.0f, 2.0f, 0.0f) ||
        path[2] != glm::vec3(3.0f, 3.0f, 0.0f)) {

        return -1;
    }

    return 0;
}
