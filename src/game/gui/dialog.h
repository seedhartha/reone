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

#include "../camera/dialogcamera.h"
#include "../object/creature.h"

#include "conversation.h"

namespace reone {

namespace game {

class DialogGUI : public Conversation {
public:
    DialogGUI(Game *game);

    void load() override;
    void update(float dt) override;

private:
    struct Participant {
        std::shared_ptr<graphics::Model> model;
        std::shared_ptr<Creature> creature;
    };

    std::shared_ptr<SpatialObject> _currentSpeaker;
    std::map<std::string, Participant> _participantByTag;

    void onStart() override;
    void onFinish() override;
    void onLoadEntry() override;
    void onEntryEnded() override;

    void setMessage(std::string message) override;

    void addFrame(std::string tag, int top, int height);
    void configureMessage();
    void configureReplies();
    void repositionMessage();

    void updateCamera();
    void updateParticipantAnimations();

    glm::vec3 getTalkPosition(const SpatialObject &object) const;
    DialogCamera::Variant getRandomCameraVariant() const;
    std::string getStuntAnimationName(int ordinal) const;
    AnimationType getStuntAnimationType(int ordinal) const;

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
