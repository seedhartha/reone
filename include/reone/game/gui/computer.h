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

#include "reone/gui/control/label.h"
#include "reone/gui/control/listbox.h"

#include "conversation.h"

namespace reone {

namespace game {

class ComputerGUI : public Conversation {
public:
    ComputerGUI(Game &game, ServicesView &services) :
        Conversation(game, services) {
        _resRef = guiResRef("computer");
    }

private:
    struct Controls {
        std::shared_ptr<gui::Label> LBL_BAR1;
        std::shared_ptr<gui::Label> LBL_BAR2;
        std::shared_ptr<gui::Label> LBL_BAR3;
        std::shared_ptr<gui::Label> LBL_BAR4;
        std::shared_ptr<gui::Label> LBL_BAR5;
        std::shared_ptr<gui::Label> LBL_BAR6;
        std::shared_ptr<gui::Label> LBL_COMP_SKILL;
        std::shared_ptr<gui::Label> LBL_COMP_SKILL_ICON;
        std::shared_ptr<gui::Label> LBL_COMP_SKILL_VAL;
        std::shared_ptr<gui::Label> LBL_COMP_SPIKES;
        std::shared_ptr<gui::Label> LBL_COMP_SPIKES_ICON;
        std::shared_ptr<gui::Label> LBL_COMP_SPIKES_VAL;
        std::shared_ptr<gui::Label> LBL_OBSCURE;
        std::shared_ptr<gui::Label> LBL_REP_SKILL;
        std::shared_ptr<gui::Label> LBL_REP_SKILL_ICON;
        std::shared_ptr<gui::Label> LBL_REP_SKILL_VAL;
        std::shared_ptr<gui::Label> LBL_REP_UNITS;
        std::shared_ptr<gui::Label> LBL_REP_UNITS_ICON;
        std::shared_ptr<gui::Label> LBL_REP_UNITS_VAL;
        std::shared_ptr<gui::Label> LBL_STATIC1;
        std::shared_ptr<gui::Label> LBL_STATIC2;
        std::shared_ptr<gui::Label> LBL_STATIC3;
        std::shared_ptr<gui::Label> LBL_STATIC4;
        std::shared_ptr<gui::ListBox> LB_MESSAGE;
        std::shared_ptr<gui::ListBox> LB_REPLIES;
    };

    Controls _controls;

    void preload(gui::IGUI &gui) override;
    void onGUILoaded() override;

    void bindControls() {
        _controls.LBL_BAR1 = findControl<gui::Label>("LBL_BAR1");
        _controls.LBL_BAR2 = findControl<gui::Label>("LBL_BAR2");
        _controls.LBL_BAR3 = findControl<gui::Label>("LBL_BAR3");
        _controls.LBL_BAR4 = findControl<gui::Label>("LBL_BAR4");
        _controls.LBL_BAR5 = findControl<gui::Label>("LBL_BAR5");
        _controls.LBL_BAR6 = findControl<gui::Label>("LBL_BAR6");
        _controls.LBL_COMP_SKILL = findControl<gui::Label>("LBL_COMP_SKILL");
        _controls.LBL_COMP_SKILL_ICON = findControl<gui::Label>("LBL_COMP_SKILL_ICON");
        _controls.LBL_COMP_SKILL_VAL = findControl<gui::Label>("LBL_COMP_SKILL_VAL");
        _controls.LBL_COMP_SPIKES = findControl<gui::Label>("LBL_COMP_SPIKES");
        _controls.LBL_COMP_SPIKES_ICON = findControl<gui::Label>("LBL_COMP_SPIKES_ICON");
        _controls.LBL_COMP_SPIKES_VAL = findControl<gui::Label>("LBL_COMP_SPIKES_VAL");
        _controls.LBL_OBSCURE = findControl<gui::Label>("LBL_OBSCURE");
        _controls.LBL_REP_SKILL = findControl<gui::Label>("LBL_REP_SKILL");
        _controls.LBL_REP_SKILL_ICON = findControl<gui::Label>("LBL_REP_SKILL_ICON");
        _controls.LBL_REP_SKILL_VAL = findControl<gui::Label>("LBL_REP_SKILL_VAL");
        _controls.LBL_REP_UNITS = findControl<gui::Label>("LBL_REP_UNITS");
        _controls.LBL_REP_UNITS_ICON = findControl<gui::Label>("LBL_REP_UNITS_ICON");
        _controls.LBL_REP_UNITS_VAL = findControl<gui::Label>("LBL_REP_UNITS_VAL");
        _controls.LBL_STATIC1 = findControl<gui::Label>("LBL_STATIC1");
        _controls.LBL_STATIC2 = findControl<gui::Label>("LBL_STATIC2");
        _controls.LBL_STATIC3 = findControl<gui::Label>("LBL_STATIC3");
        _controls.LBL_STATIC4 = findControl<gui::Label>("LBL_STATIC4");
        _controls.LB_MESSAGE = findControl<gui::ListBox>("LB_MESSAGE");
        _controls.LB_REPLIES = findControl<gui::ListBox>("LB_REPLIES");
    }

    void configureMessage();
    void configureReplies();

    void setMessage(std::string message) override;
    void setReplyLines(std::vector<std::string> lines) override;
};

} // namespace game

} // namespace reone
