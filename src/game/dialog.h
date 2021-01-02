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

#include <string>
#include <vector>

#include "../resource/gfffile.h"

namespace reone {

namespace game {

enum DialogWaitFlags {
    kDialogWaitAnimFinish = 1
};

class Dialog {
public:
    struct EntryReplyLink {
        int index { 0 };
        std::string active;
    };

    struct EntryReply {
        std::string speaker;
        std::string text;
        std::string voResRef;
        std::string script;
        std::string sound;
        std::string listener;
        int delay { 0 };
        int waitFlags { 0 };
        int cameraId { -1 };
        int cameraAngle { 0 };
        int cameraAnimation { 0 };
        float camFieldOfView { 0.0f };
        std::vector<EntryReplyLink> replies;
        std::vector<EntryReplyLink> entries;
    };

    Dialog() = default;

    void reset();
    void load(const std::string &resRef, const resource::GffStruct &dlg);

    bool isSkippable() const;
    const std::string &cameraModel() const;
    const std::vector<EntryReplyLink> &startEntries() const;
    const EntryReply &getEntry(int index) const;
    const EntryReply &getReply(int index) const;
    const std::string &endScript() const;

private:
    bool _skippable { false };
    std::string _cameraModel;
    std::vector<EntryReplyLink> _startEntries;
    std::vector<EntryReply> _entries;
    std::vector<EntryReply> _replies;
    std::string _endScript;
    int _entryIndex { -1 };

    Dialog(const Dialog &) = delete;
    Dialog &operator=(const Dialog &) = delete;

    EntryReplyLink getEntryReplyLink(const resource::GffStruct &gffs) const;
    EntryReply getEntryReply(const resource::GffStruct &gffs) const;
};

} // namespace game

} // namespace reone
