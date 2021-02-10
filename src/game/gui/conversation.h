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

#include "../../audio/soundhandle.h"
#include "../../common/timer.h"
#include "../../render/lip/lipanimation.h"
#include "../../render/model/model.h"

#include "../camera/types.h"
#include "../dialog.h"
#include "../object/spatial.h"

#include "gui.h"

namespace reone {

namespace game {

class Game;

/**
 * Base GUI for conversations.
 *
 * @see DialogGUI
 * @see Computer
 */
class Conversation : public GameGUI {
public:
    Conversation(Game *game);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;

    /**
     * Starts the specified conversation.
     *
     * @param owner owner of the conversation
     */
    void start(const std::shared_ptr<Dialog> &dialog, const std::shared_ptr<SpatialObject> &owner);

    /**
     * Get camera used in this conversation.
     *
     * @param cameraId will be set to camera ID, if animated camera is used
     */
    CameraType getCamera(int &cameraId) const;

protected:
    Game *_game;
    std::string _repliesControlTag;

    std::shared_ptr<Dialog> _dialog;
    std::shared_ptr<SpatialObject> _owner;
    std::shared_ptr<render::Model> _cameraModel;
    const Dialog::EntryReply *_currentEntry { nullptr };
    bool _entryEnded { false };

    virtual void onStart();
    virtual void onFinish();
    virtual void onLoadEntry();
    virtual void onEntryEnded();

    /**
    * @param index index of the entry in the DLG file
    * @param start true if this is a starting entry, false otherwise
    */
    virtual void loadEntry(int index, bool start = false);

private:
    std::shared_ptr<audio::SoundHandle> _currentVoice;
    std::shared_ptr<render::LipAnimation> _lipAnimation;
    Timer _endEntryTimer;
    float _entryDuration { 0.0f };
    std::vector<const Dialog::EntryReply *> _replies;
    bool _autoPickFirstReply { false };

    void onListBoxItemClick(const std::string &control, const std::string &item) override;

    void loadConversationBackground();
    void loadCameraModel();
    void loadStartEntry();
    void loadVoiceOver();
    void scheduleEndOfEntry();
    void loadReplies();

    /**
     * Replaces text in the message control.
     */
    virtual void setMessage(std::string message) = 0;

    /**
     * Recreates items in the replies list box.
     */
    void refreshReplies();

    void finish();
    void endCurrentEntry();
    void pickReply(int index);

    /**
     * @return index of the first active entry/reply from the specified list, -1 otherwise
     */
    int indexOfFirstActive(const std::vector<Dialog::EntryReplyLink> &links);

    /**
     * Executes the specified script and checks its return value.
     *
     * @return true if the script returns -1 or 1, false otherwise
     */
    bool evaluateCondition(const std::string &scriptResRef);

    // Event handlers

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);

    // END Event handlers
};

} // namespace game

} // namespace reone
