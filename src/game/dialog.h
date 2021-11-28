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

#include "../../resource/gffstruct.h"

#include "types.h"

namespace reone {

namespace resource {

class Strings;

}

namespace game {

struct DialogWaitFlags {
    static constexpr int waitAnimFinish = 1;
};

class Dialog : boost::noncopyable {
public:
    struct Stunt {
        std::string participant;
        std::string stuntModel;
    };

    struct EntryReplyLink {
        int index {0};
        std::string active;
    };

    struct ParticipantAnimation {
        std::string participant;
        int animation;
    };

    struct EntryReply {
        std::string speaker;
        std::string text;
        std::string voResRef;
        std::string script;
        std::string sound;
        std::string listener;
        int delay {0};
        int waitFlags {0};
        int cameraId {-1};
        int cameraAngle {0};
        int cameraAnimation {0};
        float camFieldOfView {0.0f};
        std::vector<EntryReplyLink> replies;
        std::vector<EntryReplyLink> entries;
        std::vector<ParticipantAnimation> animations;
    };

    Dialog(std::string resRef, resource::Strings &strings) :
        _resRef(std::move(resRef)),
        _strings(strings) {
    }

    void load(const resource::GffStruct &dlg);

    bool isSkippable() const { return _skippable; }
    bool isAnimatedCutscene() const { return _animatedCutscene; }

    const EntryReply &getEntry(int index) const;
    const EntryReply &getReply(int index) const;

    const std::string &resRef() const { return _resRef; }
    const std::string &cameraModel() const { return _cameraModel; }
    const std::vector<EntryReplyLink> &startEntries() const { return _startEntries; }
    const std::vector<Stunt> &stunts() const { return _stunts; }
    const std::string &endScript() const { return _endScript; }
    ConversationType conversationType() const { return _conversationType; }
    ComputerType computerType() const { return _computerType; }

private:
    std::string _resRef;
    resource::Strings &_strings;

    bool _skippable {false};
    std::string _cameraModel;
    std::vector<EntryReplyLink> _startEntries;
    std::vector<EntryReply> _entries;
    std::vector<EntryReply> _replies;
    std::string _endScript;
    int _entryIndex {-1};
    bool _animatedCutscene {false};
    std::vector<Stunt> _stunts;
    ConversationType _conversationType {ConversationType::Cinematic};
    ComputerType _computerType {ComputerType::Normal};

    EntryReplyLink getEntryReplyLink(const resource::GffStruct &gffs) const;
    EntryReply getEntryReply(const resource::GffStruct &gffs) const;
    Stunt getStunt(const resource::GffStruct &gffs) const;
    ParticipantAnimation getParticipantAnimation(const resource::GffStruct &gffs) const;
};

} // namespace game

} // namespace reone
