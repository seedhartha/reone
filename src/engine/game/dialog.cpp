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

#include "dialog.h"

#include "../common/guardutil.h"
#include "../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

Dialog::Dialog(string resRef, Strings *strings) :
    _resRef(move(resRef)),
    _strings(ensurePresent(strings, "strings")) {
}

void Dialog::load(const GffStruct &dlg) {
    _skippable = dlg.getBool("Skippable");
    _cameraModel = dlg.getString("CameraModel");
    _endScript = dlg.getString("EndConversation");
    _animatedCutscene = dlg.getBool("AnimatedCut");
    _conversationType = dlg.getEnum("ConversationType", ConversationType::Cinematic);
    _computerType = dlg.getEnum("ComputerType", ComputerType::Normal);

    for (auto &entry : dlg.getList("EntryList")) {
        _entries.push_back(getEntryReply(*entry));
    }
    for (auto &reply : dlg.getList("ReplyList")) {
        _replies.push_back(getEntryReply(*reply));
    }
    for (auto &entry : dlg.getList("StartingList")) {
        _startEntries.push_back(getEntryReplyLink(*entry));
    }
    for (auto &stunt : dlg.getList("StuntList")) {
        _stunts.push_back(getStunt(*stunt));
    }
}

Dialog::EntryReplyLink Dialog::getEntryReplyLink(const GffStruct &gffs) const {
    EntryReplyLink link;
    link.index = gffs.getInt("Index");
    link.active = gffs.getString("Active");

    return move(link);
}

Dialog::EntryReply Dialog::getEntryReply(const GffStruct &gffs) const {
    int strRef = gffs.getInt("Text");

    EntryReply entry;
    entry.speaker = gffs.getString("Speaker");
    entry.text = strRef == -1 ? "" : _strings->get(strRef);
    entry.voResRef = gffs.getString("VO_ResRef");
    entry.script = gffs.getString("Script");
    entry.sound = gffs.getString("Sound");
    entry.listener = gffs.getString("Listener");
    entry.delay = gffs.getInt("Delay");
    entry.waitFlags = gffs.getInt("WaitFlags");
    entry.cameraId = gffs.getInt("CameraID", -1);
    entry.cameraAngle = gffs.getInt("CameraAngle");
    entry.cameraAnimation = gffs.getInt("CameraAnimation", 0);
    entry.camFieldOfView = gffs.getFloat("CamFieldOfView", 0.0f);

    boost::to_lower(entry.speaker);
    boost::to_lower(entry.listener);

    for (auto &link : gffs.getList("RepliesList")) {
        entry.replies.push_back(getEntryReplyLink(*link));
    }
    for (auto &link : gffs.getList("EntriesList")) {
        entry.entries.push_back(getEntryReplyLink(*link));
    }
    for (auto &anim : gffs.getList("AnimList")) {
        entry.animations.push_back(getParticipantAnimation(*anim));
    }

    return move(entry);
}

Dialog::Stunt Dialog::getStunt(const GffStruct &gffs) const {
    Stunt stunt;
    stunt.participant = boost::to_lower_copy(gffs.getString("Participant"));
    stunt.stuntModel = boost::to_lower_copy(gffs.getString("StuntModel"));
    return move(stunt);
}

Dialog::ParticipantAnimation Dialog::getParticipantAnimation(const GffStruct &gffs) const {
    ParticipantAnimation anim;
    anim.participant = boost::to_lower_copy(gffs.getString("Participant"));
    anim.animation = gffs.getInt("Animation");
    return move(anim);
}

const Dialog::EntryReply &Dialog::getEntry(int index) const {
    return _entries[index];
}

const Dialog::EntryReply &Dialog::getReply(int index) const {
    return _replies[index];
}

} // namespace resource

} // namespace reone
