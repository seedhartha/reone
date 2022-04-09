/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../gui/control/label.h"
#include "../../gui/control/listbox.h"

#include "conversation.h"

namespace reone {

namespace kotor {

class ComputerGUI : public Conversation {
public:
    ComputerGUI(KotOR &game, game::Services &services);

    void load() override;

private:
    struct Binding {
        std::shared_ptr<gui::Label> lblCompSkill;
        std::shared_ptr<gui::Label> lblCompSkillVal;
        std::shared_ptr<gui::Label> lblCompSpikes;
        std::shared_ptr<gui::Label> lblCompSpikesVal;
        std::shared_ptr<gui::Label> lblRepSkill;
        std::shared_ptr<gui::Label> lblRepSkillVal;
        std::shared_ptr<gui::Label> lblRepUnits;
        std::shared_ptr<gui::Label> lblRepUnitsVal;
        std::shared_ptr<gui::ListBox> lbMessage;
        std::shared_ptr<gui::ListBox> lbReplies;

        // KotOR only
        std::shared_ptr<gui::Label> lblCompSkillIcon;
        std::shared_ptr<gui::Label> lblCompSpikesIcon;
        std::shared_ptr<gui::Label> lblRepSkillIcon;
        std::shared_ptr<gui::Label> lblRepUnitsIcon;
        std::shared_ptr<gui::Label> lblStatic1;
        std::shared_ptr<gui::Label> lblStatic2;
        std::shared_ptr<gui::Label> lblStatic3;
        std::shared_ptr<gui::Label> lblStatic4;
        std::shared_ptr<gui::Label> lblObscure;
        // END KotOR only

        // TSL only
        std::shared_ptr<gui::Label> lblBar1;
        std::shared_ptr<gui::Label> lblBar2;
        std::shared_ptr<gui::Label> lblBar3;
        std::shared_ptr<gui::Label> lblBar4;
        std::shared_ptr<gui::Label> lblBar5;
        std::shared_ptr<gui::Label> lblBar6;
        // END TSL only
    } _binding;

    void bindControls();
    void configureMessage();
    void configureReplies();

    void setMessage(std::string message) override;
    void setReplyLines(std::vector<std::string> lines) override;
};

} // namespace kotor

} // namespace reone
