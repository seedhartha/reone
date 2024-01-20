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

#pragma once

#include "camerastyle.h"

namespace reone {

namespace resource {

class TwoDAs;

}

namespace game {

class ICameraStyles {
public:
    virtual ~ICameraStyles() = default;

    virtual std::shared_ptr<CameraStyle> get(int index) const = 0;
    virtual std::shared_ptr<CameraStyle> get(const std::string &name) const = 0;
};

class CameraStyles : public ICameraStyles, boost::noncopyable {
public:
    CameraStyles(resource::TwoDAs &twoDas) :
        _twoDas(twoDas) {
    }

    void init();

    std::shared_ptr<CameraStyle> get(int index) const override;
    std::shared_ptr<CameraStyle> get(const std::string &name) const override;

private:
    resource::TwoDAs &_twoDas;

    std::vector<std::shared_ptr<CameraStyle>> _styles;
};

} // namespace game

} // namespace reone
