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

#ifdef _WIN32
#include <windows.h>
#endif

#include "reone/system/logger.h"
#include "reone/system/threadutil.h"

#include "view/resource/explorerframe.h"

namespace reone {

bool ToolkitApp::OnInit() {
#ifdef _WIN32
    SetProcessDPIAware();
#endif
    markMainThread();
    Logger::instance.init(
        LogSeverity::Debug,
        std::set<LogChannel> {LogChannel::Global, LogChannel::Resources, LogChannel::Graphics, LogChannel::Audio},
        "toolkit.log");

    wxImage::AddHandler(new wxTGAHandler());

    auto frame = new ResourceExplorerFrame();
    frame->Show();

    return true;
};

} // namespace reone

wxIMPLEMENT_APP(reone::ToolkitApp);
