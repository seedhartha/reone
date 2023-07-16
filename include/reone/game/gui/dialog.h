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

#include "../object/camera/dialog.h"
#include "../object/creature.h"

#include "conversation.h"

namespace reone {

namespace game {

class DialogGUI : public Conversation {
public:
    DialogGUI(Game &game, ServicesView &services) :
        Conversation(game, services) {
        _resRef = guiResRef("dialog");
    }

    void update(float dt) override;

private:
    struct Participant {
        std::shared_ptr<graphics::Model> model;
        std::shared_ptr<Creature> creature;
    };

    struct Controls {
        std::shared_ptr<gui::Label> LBL_MESSAGE;
        std::shared_ptr<gui::ListBox> LB_REPLIES;
    };

    Controls _controls;

    std::shared_ptr<Object> _currentSpeaker;
    std::map<std::string, Participant> _participantByTag;

    void preload(gui::IGUI &gui) override;
    void onGUILoaded() override;

    void bindControls() {
        _controls.LBL_MESSAGE = findControl<gui::Label>("LBL_MESSAGE");
        _controls.LB_REPLIES = findControl<gui::ListBox>("LB_REPLIES");
    }

    void addFrame(std::string tag, int top, int height);
    void configureMessage();
    void configureReplies();
    void repositionMessage();

    void updateCamera();
    void updateParticipantAnimations();

    glm::vec3 getTalkPosition(const Object &object) const;
    DialogCamera::Variant getRandomCameraVariant() const;
    std::string getStuntAnimationName(int ordinal) const;
    AnimationType getStuntAnimationType(int ordinal) const;

    void setMessage(std::string message) override;
    void setReplyLines(std::vector<std::string> lines) override;

    void onStart() override;
    void onFinish() override;
    void onLoadEntry() override;
    void onEntryEnded() override;

    // Loading

    void loadFrames();
    void loadCurrentSpeaker();

    // END Loading

    // Participants

    void loadStuntParticipants();
    void releaseStuntParticipants();

    // END Participants
};

} // namespace game

} // namespace reone
