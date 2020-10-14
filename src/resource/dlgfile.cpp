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

#include "dlgfile.h"

#include <boost/algorithm/string.hpp>

#include "resources.h"

using namespace std;

namespace reone {

namespace resource {

void DlgFile::reset() {
    _entries.clear();
    _replies.clear();
    _startEntries.clear();
    _endScript.clear();
}

void DlgFile::load(const string &resRef, const GffStruct &dlg) {
    _skippable = dlg.getInt("Skippable") != 0;
    _endScript = dlg.getString("EndConversation");

    for (auto &entry : dlg.getList("EntryList")) {
        _entries.push_back(getEntryReply(entry));
    }
    for (auto &reply : dlg.getList("ReplyList")) {
        _replies.push_back(getEntryReply(reply));
    }
    for (auto &entry : dlg.getList("StartingList")) {
        _startEntries.push_back(getEntryReplyLink(entry));
    }
}

DlgFile::EntryReplyLink DlgFile::getEntryReplyLink(const GffStruct &gffs) const {
    EntryReplyLink link;
    link.index = gffs.getInt("Index");
    link.active = gffs.getString("Active");

    return move(link);
}

DlgFile::EntryReply DlgFile::getEntryReply(const GffStruct &gffs) const {
    int strRef = gffs.getInt("Text");

    EntryReply entry;
    entry.speaker = gffs.getString("Speaker");
    entry.text = strRef == -1 ? "" : Resources.getString(strRef).text;
    entry.voResRef = gffs.getString("VO_ResRef");
    entry.script = gffs.getString("Script");
    entry.sound = gffs.getString("Sound");
    entry.listener = gffs.getString("Listener");
    entry.delay = gffs.getInt("Delay");
    entry.waitFlags = gffs.getInt("WaitFlags");
    entry.cameraAngle = gffs.getInt("CameraAngle");

    boost::to_lower(entry.speaker);
    boost::to_lower(entry.listener);

    const GffField *repliesList = gffs.find("RepliesList");
    if (repliesList) {
        for (auto &link : repliesList->children()) {
            entry.replies.push_back(getEntryReplyLink(link));
        }
    }
    const GffField *entriesList = gffs.find("EntriesList");
    if (entriesList) {
        for (auto &link : entriesList->children()) {
            entry.entries.push_back(getEntryReplyLink(link));
        }
    }

    return move(entry);
}

bool DlgFile::isSkippable() const {
    return _skippable;
}

const vector<DlgFile::EntryReplyLink> &DlgFile::startEntries() const {
    return _startEntries;
}

const DlgFile::EntryReply &DlgFile::getEntry(int index) const {
    return _entries[index];
}

const DlgFile::EntryReply &DlgFile::getReply(int index) const {
    return _replies[index];
}

const string &DlgFile::endScript() const {
    return _endScript;
}

} // namespace resource

} // namespace reone
