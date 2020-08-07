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

#include "dialog.h"

#include <boost/algorithm/string.hpp>

#include "../resources/manager.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace game {

void Dialog::reset() {
    _entries.clear();
    _replies.clear();
    _startEntries.clear();
}

void Dialog::load(const string &resRef, const GffStruct &dlg) {
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
    entry.text = strRef == -1 ? "" : ResMan.getString(strRef).text;
    entry.voResRef = gffs.getString("VO_ResRef");
    entry.script = gffs.getString("Script");
    entry.sound = gffs.getString("Sound");
    entry.listener = gffs.getString("Listener");
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

const vector<Dialog::EntryReplyLink> &Dialog::startEntries() const {
    return _startEntries;
}

const Dialog::EntryReply &Dialog::getEntry(int index) const {
    assert(index >= 0 && index < _entries.size());
    return _entries[index];
}

const Dialog::EntryReply &Dialog::getReply(int index) const {
    assert(index >= 0 && index < _replies.size());
    return _replies[index];
}

} // namespace game

} // namespace reone
