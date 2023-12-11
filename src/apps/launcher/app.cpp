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

#include "app.h"

#include "reone/system/threadutil.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "frame.h"

namespace reone {

bool LauncherApp::OnInit() {
#ifdef _WIN32
    SetProcessDPIAware();
#endif
    markMainThread();
    auto frame = new LauncherFrame(); // managed by the library
    frame->Show();
    return true;
};

} // namespace reone

wxIMPLEMENT_APP(reone::LauncherApp);
