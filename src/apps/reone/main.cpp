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

#ifdef _WIN32
#include <windows.h>
#undef max
#undef min
#endif

#include "reone/system/threadutil.h"

#include "engine.h"

using namespace reone;

int main(int argc, char **argv) {
#ifdef _WIN32
    SetProcessDPIAware();
#endif
    setMainThread();

    auto engine = Engine(argc, argv);
    try {
        engine.init();
        return engine.run();
    } catch (const std::exception &ex) {
        try {
            error("Program terminated exceptionally: " + std::string(ex.what()));
        } catch (...) {
        }
        return 1;
    }
}
