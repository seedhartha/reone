/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../common/logutil.h"

#include "engine.h"

using namespace std;

using namespace reone;
using namespace reone::engine;

extern "C" {
#ifdef _WIN32
#define R_DECLSPEC __declspec(dllexport)
#else
#define R_DECLSPEC
#endif
R_DECLSPEC uint32_t NvOptimusEnablement = 0x00000001;
R_DECLSPEC uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char **argv) {
    initLog();
    try {
        return Engine(argc, argv).run();
    } catch (const exception &ex) {
        try {
            error("Program terminated exceptionally: " + string(ex.what()));
        } catch (...) {
        }
        return 1;
    }
}
