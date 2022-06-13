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

#include "../../game/game.h"

namespace reone {

namespace game {

class MockGame : public IGame {
public:
    void startNewGame() override {
    }

    void warpToModule(const std::string &name) override {
    }

    void quit() override {
    }

    void changeCursor(CursorType type) override {
        _changeCursorInvocations.push_back(type);
    }

    void startConversation(const std::string &name) override {
    }

    const std::set<std::string> &moduleNames() const override {
        return _moduleNames;
    }

    const std::vector<CursorType> &changeCursorInvocations() const {
        return _changeCursorInvocations;
    }

private:
    std::set<std::string> _moduleNames;
    std::vector<CursorType> _changeCursorInvocations;
};

} // namespace game

} // namespace reone
