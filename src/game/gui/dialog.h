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

#include "../object/object.h"

namespace reone {

namespace game {

class DialogGui : public gui::GUI {
public:
    DialogGui(resource::GameVersion version, const render::GraphicsOptions &opts);

    void load() override;
    void startDialog(uint32_t ownerId, const std::string &resRef);
    void pickReply(uint32_t index);
    void update(float dt) override;

    void setPickReplyEnabled(bool enabled);
    void setGetObjectIdByTagFunc(const std::function<uint32_t(const std::string &)> &fn);
    void setOnReplyPicked(const std::function<void(uint32_t)> &fn);
    void setOnSpeakerChanged(const std::function<void(uint32_t, uint32_t)> &fn);
    void setOnDialogFinished(const std::function<void()> &fn);

private:
    uint32_t _ownerId { 0 };
    std::shared_ptr<resource::DlgFile> _dialog;
    std::shared_ptr<resource::DlgFile::EntryReply> _currentEntry;
    std::shared_ptr<audio::SoundInstance> _currentVoice;
    uint32_t _currentSpeaker { 0 };
    bool _pickReplyEnabled { true };
    int _autoPickReplyIdx { -1 };
    int _autoPickReplyFlags { 0 };
    uint32_t _autoPickReplyTimestamp { 0 };

    // Callbacks

    std::function<uint32_t(const std::string &)> _getObjectIdByTag;
    std::function<void(uint32_t)> _onReplyPicked;
    std::function<void(uint32_t, uint32_t)> _onSpeakerChanged;
    std::function<void()> _onDialogFinished;

    // END Callbacks

    bool handleKeyDown(SDL_Scancode key) override;
    bool handleKeyUp(SDL_Scancode key) override;

    void addTopFrame();
    void addBottomFrame();
    void addFrame(int top, int height);
    void configureMessage();
    void configureReplies();
    void onReplyClicked(int index);
    void loadStartEntry();
    bool checkCondition(const std::string &script);
    void loadCurrentEntry();
    void loadCurrentSpeaker();
    void playVoiceOver();
    void loadReplies();
    void scheduleReplyPick(int replyIdx);
    void finish();
};

} // namespace game

} // namespace reone
