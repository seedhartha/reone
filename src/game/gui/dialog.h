/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../audio/soundinstance.h"
#include "../../gui/gui.h"
#include "../../resource/dlgfile.h"
#include "../../resource/types.h"

#include "../camera/dialogcamera.h"
#include "../object/spatial.h"

namespace reone {

namespace game {

class Game;

class DialogGui : public gui::GUI {
public:
    DialogGui(resource::GameVersion version, Game *game, const render::GraphicsOptions &opts);

    void load() override;
    void startDialog(SpatialObject &owner, const std::string &resRef);
    void pickReply(uint32_t index);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;

    Camera &camera() const;

    void setPickReplyEnabled(bool enabled);
    void setOnReplyPicked(const std::function<void(uint32_t)> &fn);
    void setOnDialogFinished(const std::function<void()> &fn);

private:
    Game *_game { nullptr };
    SpatialObject *_owner { nullptr };
    std::shared_ptr<resource::DlgFile> _dialog;
    std::shared_ptr<resource::DlgFile::EntryReply> _currentEntry;
    std::shared_ptr<audio::SoundInstance> _currentVoice;
    SpatialObject *_currentSpeaker { nullptr };
    bool _pickReplyEnabled { true };
    int _autoPickReplyIdx { -1 };
    int _endEntryFlags { 0 };
    uint32_t _endEntryTimestamp { 0 };
    bool _entryEnded { false };

    // Callbacks

    std::function<void(uint32_t)> _onReplyPicked;
    std::function<void()> _onDialogFinished;

    // END Callbacks

    bool handleKeyDown(SDL_Scancode key) override;
    bool handleKeyUp(SDL_Scancode key) override;

    // Loading

    void loadTopFrame();
    void loadBottomFrame();

    // END Loading

    void addFrame(int top, int height);
    void configureMessage();
    void configureReplies();
    void loadAnimatedCamera();
    void loadStartEntry();
    bool checkCondition(const std::string &script);
    void loadCurrentEntry();
    void loadReplies();
    void loadCurrentSpeaker();
    void updateCamera();
    DialogCamera::Variant getRandomCameraVariant() const;
    void playVoiceOver();
    void scheduleEndOfEntry();
    void finish();
    void endCurrentEntry();

    void onReplyClicked(int index);
};

} // namespace game

} // namespace reone
