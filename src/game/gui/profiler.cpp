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

#include "profiler.h"

#include "../../graphics/fonts.h"
#include "../../graphics/options.h"
#include "../../graphics/services.h"
#include "../../gui/control/listbox.h"
#include "../../gui/control/panel.h"

#include "../profiler.h"

using namespace std;

using namespace reone::gui;

namespace reone {

namespace game {

static constexpr int kPanelWidth = 200;
static constexpr float kFlushInterval = 1.0f;

static const string kFontResRef = "fnt_console";

void ProfilerGui::init() {
    auto font = _graphicsSvc.fonts.get(kFontResRef);

    // Text list box

    auto lbTextProtoItem = newLabel(ListBox::itemControlId(0, -1));
    lbTextProtoItem->setExtent(glm::ivec4(_graphicsOpt.width - kPanelWidth, 0, kPanelWidth, font->height()));
    lbTextProtoItem->setFont(kFontResRef);

    auto lbText = static_pointer_cast<ListBox>(newListBox(0));
    lbText->setExtent(glm::ivec4(_graphicsOpt.width - kPanelWidth, 0, kPanelWidth, _graphicsOpt.height));
    lbText->setProtoItem(lbTextProtoItem.get());
    lbText->initItemSlots();
    _lbText = lbText.get();

    // Root control

    auto rootControl = static_pointer_cast<Panel>(newPanel(-1));
    rootControl->setExtent(glm::ivec4(_graphicsOpt.width - kPanelWidth, 0, kPanelWidth, _graphicsOpt.height));
    rootControl->setBorderFill("black");
    rootControl->setAlpha(0.5f);
    rootControl->append(*_lbText);
    _rootControl = rootControl.get();
}

void ProfilerGui::update(float delta) {
    if (!_enabled) {
        return;
    }
    auto strings = vector<string>();
    strings.push_back("FPS:");
    strings.push_back("avg " + to_string(_profiler.averageFps()));
    strings.push_back("0.1% low " + to_string(_profiler.onePercentLowFps()));
    strings.push_back("");
    strings.push_back("Frame time:");
    strings.push_back(str(boost::format("input %.04f") % _profiler.inputTimes().back()));
    strings.push_back(str(boost::format("update %.04f") % _profiler.updateTimes().back()));
    strings.push_back(str(boost::format("render %.04f") % _profiler.renderTimes().back()));
    _lbText->clearItems();
    for (auto &s : strings) {
        _lbText->appendItem(ListBox::Item {s});
    }
}

} // namespace game

} // namespace reone
