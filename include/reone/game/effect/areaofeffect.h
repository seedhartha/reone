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

#include "../effect.h"

namespace reone {

namespace game {

class AreaOfEffectEffect : public Effect {
public:
    AreaOfEffectEffect(int areaEffectId,
                       std::string onEnterScript,
                       std::string heartbeatScript,
                       std::string onExitScript) :
        Effect(EffectType::AreaOfEffect),
        _areaEffectId(areaEffectId),
        _onEnterScript(std::move(onEnterScript)),
        _heartbeatScript(std::move(heartbeatScript)),
        _onExitScript(std::move(onExitScript)) {
    }

    void applyTo(Object &object) override {
    }

private:
    int _areaEffectId;
    std::string _onEnterScript;
    std::string _heartbeatScript;
    std::string _onExitScript;
};

} // namespace game

} // namespace reone
