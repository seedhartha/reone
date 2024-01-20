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

#include "reone/game/camerastyles.h"

#include "reone/resource/2da.h"
#include "reone/resource/provider/2das.h"

using namespace reone::resource;

namespace reone {

namespace game {

void CameraStyles::init() {
    std::shared_ptr<TwoDA> twoDa(_twoDas.get("camerastyle"));
    if (!twoDa) {
        return;
    }
    for (int row = 0; row < twoDa->getRowCount(); ++row) {
        auto style = std::make_shared<CameraStyle>();
        style->name = twoDa->getString(row, "name");
        style->distance = twoDa->getFloat(row, "distance");
        style->pitch = twoDa->getFloat(row, "pitch");
        style->viewAngle = twoDa->getFloat(row, "viewangle");
        style->height = twoDa->getFloat(row, "height");
        _styles.push_back(std::move(style));
    }
}

std::shared_ptr<CameraStyle> CameraStyles::get(int index) const {
    return _styles[index];
}

std::shared_ptr<CameraStyle> CameraStyles::get(const std::string &name) const {
    for (auto &style : _styles) {
        if (style->name == name) {
            return style;
        }
    }
    return nullptr;
}

} // namespace game

} // namespace reone
