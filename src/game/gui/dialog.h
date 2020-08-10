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
#include "../../resources/types.h"

#include "../dialog.h"
#include "../object/object.h"

namespace reone {

namespace game {

class DialogGui : public gui::GUI {
public:
    DialogGui(const render::GraphicsOptions &opts);

    void load(resources::GameVersion version);
    void startDialog(const Object &owner, const std::string &resRef);

    void setOnSpeakerChanged(const std::function<void(const std::string &, const std::string &)> &fn);
    void setOnDialogFinished(const std::function<void()> &fn);

private:
    resources::GameVersion _version { resources::GameVersion::KotOR };
    const Object *_owner { nullptr };
    std::shared_ptr<Dialog> _dialog;
    std::shared_ptr<Dialog::EntryReply> _currentEntry;
    std::shared_ptr<audio::SoundInstance> _currentVoice;
    std::string _currentSpeaker;
    std::function<void(const std::string &, const std::string &)> _onSpeakerChanged;
    std::function<void()> _onDialogFinished;

    void addTopFrame();
    void addBottomFrame();
    void addFrame(int top, int height);
    void configureMessage();
    void configureReplies();
    void onReplyClicked(int index);
    void loadStartEntry();
    bool checkCondition(const std::string &script);
    void loadCurrentEntry();
    void finish();
};

} // namespace game

} // namespace reone
