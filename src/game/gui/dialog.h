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

#include "../../gui/gui.h"
#include "../../resource/types.h"

#include "../camera/dialogcamera.h"
#include "../dialog.h"
#include "../object/creature.h"

namespace reone {

namespace audio {

class SoundHandle;

}

namespace game {

class Game;

class DialogGUI : public gui::GUI {
public:
    DialogGUI(Game *game);

    void load() override;
    void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef);
    void pickReply(uint32_t index);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;

    CameraType getCamera(int &cameraId) const;

private:
    struct Participant {
        std::shared_ptr<render::Model> model;
        std::shared_ptr<Creature> creature;
    };

    Game *_game { nullptr };
    std::shared_ptr<SpatialObject> _owner;
    std::shared_ptr<Dialog> _dialog;
    std::shared_ptr<Dialog::EntryReply> _currentEntry;
    std::shared_ptr<audio::SoundHandle> _currentVoice;
    std::shared_ptr<SpatialObject> _currentSpeaker;
    std::map<std::string, Participant> _participantByTag;
    int _autoPickReplyIdx { -1 };
    int _endEntryFlags { 0 };
    float _endEntryTimeout { 0.0f };
    bool _entryEnded { false };

    bool checkCondition(const std::string &script);
    void addFrame(int top, int height);
    void configureMessage();
    void configureReplies();
    void endCurrentEntry();
    void finish();
    void playVoiceOver();
    void scheduleEndOfEntry();
    void updateCamera();
    void updateParticipantAnimations();

    void onReplyClicked(int index);

    bool handleKeyDown(SDL_Scancode key) override;
    bool handleKeyUp(SDL_Scancode key) override;

    glm::vec3 getTalkPosition(const SpatialObject &object) const;
    DialogCamera::Variant getRandomCameraVariant() const;
    std::string getStuntAnimationName(int ordinal) const;

    void onListBoxItemClick(const std::string &control, const std::string &item) override;

    // Loading

    void loadTopFrame();
    void loadBottomFrame();

    void loadAnimatedCamera();
    void loadCurrentEntry();
    void loadCurrentSpeaker();
    void loadReplies();
    void loadStartEntry();

    // END Loading

    // Participants

    void loadParticipants();
    void releaseParticipants();

    // END Participants
};

} // namespace game

} // namespace reone
