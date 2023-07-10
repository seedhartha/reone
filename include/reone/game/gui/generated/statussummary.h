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

#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/gui.h"

namespace reone {

namespace game {

class GUI_statussummary : gui::IGUI, boost::noncopyable {
public:
    void bindControls() {
        _controls.BTN_OK = findControl<gui::Button>("BTN_OK");
        _controls.LBL_CREDITS = findControl<gui::Label>("LBL_CREDITS");
        _controls.LBL_CREDITS_DESC = findControl<gui::Label>("LBL_CREDITS_DESC");
        _controls.LBL_DARKSIDE = findControl<gui::Label>("LBL_DARKSIDE");
        _controls.LBL_DARKSIDE_DESC = findControl<gui::Label>("LBL_DARKSIDE_DESC");
        _controls.LBL_INFLUENCE_LOST = findControl<gui::Label>("LBL_INFLUENCE_LOST");
        _controls.LBL_INFLUENCE_LOST_DESC = findControl<gui::Label>("LBL_INFLUENCE_LOST_DESC");
        _controls.LBL_INFLUENCE_RECV = findControl<gui::Label>("LBL_INFLUENCE_RECV");
        _controls.LBL_INFLUENCE_RECV_DESC = findControl<gui::Label>("LBL_INFLUENCE_RECV_DESC");
        _controls.LBL_JOURNAL = findControl<gui::Label>("LBL_JOURNAL");
        _controls.LBL_JOURNAL_DESC = findControl<gui::Label>("LBL_JOURNAL_DESC");
        _controls.LBL_LIGHTSIDE = findControl<gui::Label>("LBL_LIGHTSIDE");
        _controls.LBL_LIGHTSIDE_DESC = findControl<gui::Label>("LBL_LIGHTSIDE_DESC");
        _controls.LBL_LOST = findControl<gui::Label>("LBL_LOST");
        _controls.LBL_LOST_DESC = findControl<gui::Label>("LBL_LOST_DESC");
        _controls.LBL_MAX_FP_GAINED = findControl<gui::Label>("LBL_MAX_FP_GAINED");
        _controls.LBL_MAX_FP_GAINED_DESC = findControl<gui::Label>("LBL_MAX_FP_GAINED_DESC");
        _controls.LBL_MAX_FP_LOST = findControl<gui::Label>("LBL_MAX_FP_LOST");
        _controls.LBL_MAX_FP_LOST_DESC = findControl<gui::Label>("LBL_MAX_FP_LOST_DESC");
        _controls.LBL_NETSHIFT = findControl<gui::Label>("LBL_NETSHIFT");
        _controls.LBL_NETSHIFT_DESC = findControl<gui::Label>("LBL_NETSHIFT_DESC");
        _controls.LBL_RECEIVED = findControl<gui::Label>("LBL_RECEIVED");
        _controls.LBL_RECEIVED_DESC = findControl<gui::Label>("LBL_RECEIVED_DESC");
        _controls.LBL_STEALTH = findControl<gui::Label>("LBL_STEALTH");
        _controls.LBL_STEALTH_DESC = findControl<gui::Label>("LBL_STEALTH_DESC");
        _controls.LBL_XP = findControl<gui::Label>("LBL_XP");
        _controls.LBL_XP_DESC = findControl<gui::Label>("LBL_XP_DESC");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Button> BTN_OK;
        std::shared_ptr<gui::Label> LBL_CREDITS;
        std::shared_ptr<gui::Label> LBL_CREDITS_DESC;
        std::shared_ptr<gui::Label> LBL_DARKSIDE;
        std::shared_ptr<gui::Label> LBL_DARKSIDE_DESC;
        std::shared_ptr<gui::Label> LBL_INFLUENCE_LOST;
        std::shared_ptr<gui::Label> LBL_INFLUENCE_LOST_DESC;
        std::shared_ptr<gui::Label> LBL_INFLUENCE_RECV;
        std::shared_ptr<gui::Label> LBL_INFLUENCE_RECV_DESC;
        std::shared_ptr<gui::Label> LBL_JOURNAL;
        std::shared_ptr<gui::Label> LBL_JOURNAL_DESC;
        std::shared_ptr<gui::Label> LBL_LIGHTSIDE;
        std::shared_ptr<gui::Label> LBL_LIGHTSIDE_DESC;
        std::shared_ptr<gui::Label> LBL_LOST;
        std::shared_ptr<gui::Label> LBL_LOST_DESC;
        std::shared_ptr<gui::Label> LBL_MAX_FP_GAINED;
        std::shared_ptr<gui::Label> LBL_MAX_FP_GAINED_DESC;
        std::shared_ptr<gui::Label> LBL_MAX_FP_LOST;
        std::shared_ptr<gui::Label> LBL_MAX_FP_LOST_DESC;
        std::shared_ptr<gui::Label> LBL_NETSHIFT;
        std::shared_ptr<gui::Label> LBL_NETSHIFT_DESC;
        std::shared_ptr<gui::Label> LBL_RECEIVED;
        std::shared_ptr<gui::Label> LBL_RECEIVED_DESC;
        std::shared_ptr<gui::Label> LBL_STEALTH;
        std::shared_ptr<gui::Label> LBL_STEALTH_DESC;
        std::shared_ptr<gui::Label> LBL_XP;
        std::shared_ptr<gui::Label> LBL_XP_DESC;
    };

    Controls _controls;
};

} // namespace game

} // namespace reone
