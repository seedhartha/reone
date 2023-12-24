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

#include "reone/audio/source.h"
#include "reone/graphics/lipanimation.h"
#include "reone/graphics/model.h"
#include "reone/resource/dialog.h"
#include "reone/system/timer.h"

#include "../gui.h"
#include "../object.h"
#include "../types.h"

namespace reone {

namespace game {

class Conversation : public GameGUI {
public:
    Conversation(Game &game, ServicesView &services) :
        GameGUI(game, services) {
    }

    bool handle(const input::Event &event) override;
    void update(float dt) override;

    void start(const std::shared_ptr<resource::Dialog> &dialog, const std::shared_ptr<Object> &owner);

    CameraType getCamera(int &cameraId) const;

    void pause();

    void resume();

protected:
    std::shared_ptr<resource::Dialog> _dialog;
    std::shared_ptr<Object> _owner;
    std::shared_ptr<graphics::Model> _cameraModel;
    std::shared_ptr<graphics::LipAnimation> _lipAnimation;
    const resource::Dialog::EntryReply *_currentEntry {nullptr};
    bool _entryEnded {false};
    bool _paused {false};

    virtual void loadEntry(int index, bool start = false);

    void pickReply(int index);

    virtual void setReplyLines(std::vector<std::string> lines) = 0;

    virtual void onStart();
    virtual void onFinish();
    virtual void onLoadEntry();
    virtual void onEntryEnded();

private:
    std::shared_ptr<audio::AudioSource> _currentVoice;
    Timer _endEntryTimer;
    float _entryDuration {0.0f};
    std::vector<const resource::Dialog::EntryReply *> _replies;
    bool _autoPickFirstReply {false};

    void loadConversationBackground();
    void loadCameraModel();
    void loadStartEntry();
    void loadVoiceOver();
    void scheduleEndOfEntry();
    void loadReplies();

    bool isSkippableEntry() const;

    void refreshReplies();

    void finish();
    void endCurrentEntry();

    int indexOfFirstActive(const std::vector<resource::Dialog::EntryReplyLink> &links);

    bool evaluateCondition(const std::string &scriptResRef);

    virtual void setMessage(std::string message) = 0;

    // Event handlers

    bool handleMouseButtonDown(const input::MouseButtonEvent &event);
    bool handleKeyUp(const input::KeyEvent &event);

    // END Event handlers
};

} // namespace game

} // namespace reone
