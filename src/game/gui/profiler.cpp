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

#include "reone/graphics/fonts.h"
#include "reone/graphics/options.h"
#include "reone/graphics/services.h"
#include "reone/gui/control/listbox.h"
#include "reone/gui/control/panel.h"
#include "reone/gui/control/plotter.h"

#include "../profiler.h"

using namespace std;

using namespace reone::gui;

namespace reone {

namespace game {

static constexpr int kPanelWidth = 206; // 2 * 100 + 6
static constexpr int kPlotterHeight = 200;

static const string kFontResRef = "fnt_console";

void ProfilerGui::init() {
    auto font = _graphicsSvc.fonts.get(kFontResRef);

    // Text list box

    auto lbTextProtoItem = newLabel(ListBox::itemControlId(1, -1));
    lbTextProtoItem->setExtent(glm::ivec4(3 + _graphicsOpt.width - kPanelWidth, 3, kPanelWidth - 6, font->height()));
    lbTextProtoItem->setFont(kFontResRef);

    auto lbText = static_pointer_cast<ListBox>(newListBox(1));
    lbText->setExtent(glm::ivec4(3 + _graphicsOpt.width - kPanelWidth, 3, kPanelWidth - 6, 3 * font->height()));
    lbText->setProtoItem(lbTextProtoItem.get());
    lbText->initItemSlots();
    _lbText = lbText.get();

    // Frame times plotter

    auto pltFrameTimes = static_pointer_cast<Plotter>(newPlotter(0));
    pltFrameTimes->setExtent(glm::ivec4(3 + _graphicsOpt.width - kPanelWidth, 6 + 3 * font->height(), kPanelWidth - 6, kPlotterHeight));
    pltFrameTimes->setAxes(glm::vec4(0.0f, 0.0042f, 100.0f, 0.025f));
    _pltFrameTimes = pltFrameTimes.get();

    // Root control

    auto rootControl = static_pointer_cast<Panel>(newPanel(-1));
    rootControl->setExtent(glm::ivec4(_graphicsOpt.width - kPanelWidth, 0, kPanelWidth, _graphicsOpt.height));
    rootControl->setBorderFill("black");
    rootControl->setAlpha(0.5f);
    rootControl->append(*_pltFrameTimes);
    rootControl->append(*_lbText);
    _rootControl = rootControl.get();
}

void ProfilerGui::update(float delta) {
    if (!_enabled) {
        return;
    }

    _pltFrameTimes->clearFigures();

    // Frame times

    auto &frameTimes = _profiler.frameTimes();
    auto frameTimesFigure = Plotter::Figure();
    frameTimesFigure.color = glm::vec3(1.0f, 1.0f, 1.0f);
    for (auto &time : frameTimes) {
        auto &points = frameTimesFigure.points;
        points.push_back(glm::vec2(static_cast<float>(points.size()), time));
    }
    _pltFrameTimes->addFigure(move(frameTimesFigure));

    // Input times

    auto &inputTimes = _profiler.inputTimes();
    auto inputTimesFigure = Plotter::Figure();
    inputTimesFigure.color = glm::vec3(0.0f, 0.0f, 1.0f);
    for (auto &time : inputTimes) {
        auto &points = inputTimesFigure.points;
        points.push_back(glm::vec2(static_cast<float>(points.size()), time));
    }
    _pltFrameTimes->addFigure(move(inputTimesFigure));

    // Update times

    auto &updateTimes = _profiler.updateTimes();
    auto updateTimesFigure = Plotter::Figure();
    updateTimesFigure.color = glm::vec3(0.0f, 1.0f, 0.0f);
    for (auto &time : updateTimes) {
        auto &points = updateTimesFigure.points;
        points.push_back(glm::vec2(static_cast<float>(points.size()), time));
    }
    _pltFrameTimes->addFigure(move(updateTimesFigure));

    // Render times

    auto &renderTimes = _profiler.renderTimes();
    auto renderTimesFigure = Plotter::Figure();
    renderTimesFigure.color = glm::vec3(1.0f, 0.0f, 0.0f);
    for (auto &time : renderTimes) {
        auto &points = renderTimesFigure.points;
        points.push_back(glm::vec2(static_cast<float>(points.size()), time));
    }
    _pltFrameTimes->addFigure(move(renderTimesFigure));

    auto strings = vector<string>();
    strings.push_back("FPS:");
    strings.push_back("avg " + to_string(_profiler.averageFps()));
    strings.push_back("0.1% low " + to_string(_profiler.onePercentLowFps()));
    _lbText->clearItems();
    for (auto &s : strings) {
        _lbText->appendItem(ListBox::Item {s});
    }
}

} // namespace game

} // namespace reone
