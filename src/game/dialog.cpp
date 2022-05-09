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

#include "../common/collectionutil.h"
#include "../common/exception/argument.h"
#include "../common/exception/notfound.h"
#include "../common/exception/validation.h"
#include "../resource/gff.h"
#include "../resource/gffs.h"
#include "../resource/services.h"
#include "../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

static const string kEmptyText = "empty";

void Dialog::load(const string &name) {
    auto dlg = _resourceSvc.gffs.get(name, ResourceType::Dlg);
    if (!dlg) {
        throw NotFoundException("DLG not found: " + name);
    }
    loadEntries(*dlg);
    loadReplies(*dlg);
}

void Dialog::loadEntries(const Gff &dlg) {
    auto &dlgEntries = dlg.getList("EntryList");
    for (auto &dlgEntry : dlgEntries) {
        auto textStrRef = dlgEntry->getInt("Text");
        auto text = (textStrRef != -1) ? _resourceSvc.strings.get(textStrRef) : kEmptyText;

        auto replies = vector<EntryReplyLink>();
        auto dlgReplies = dlgEntry->getList("RepliesList");
        for (auto &dlgReply : dlgReplies) {
            auto index = dlgReply->getInt("Index");
            replies.push_back(EntryReplyLink {index});
        }

        auto entry = Entry();
        entry.text = move(text);
        entry.replies = move(replies);
        _entries.push_back(move(entry));
    }
}

void Dialog::loadReplies(const Gff &dlg) {
    auto &dlgReplies = dlg.getList("ReplyList");
    for (auto &dlgReply : dlgReplies) {
        auto textStrRef = dlgReply->getInt("Text");
        auto text = (textStrRef != -1) ? _resourceSvc.strings.get(textStrRef) : kEmptyText;

        auto entries = vector<ReplyEntryLink>();
        auto dlgEntries = dlgReply->getList("EntriesList");
        for (auto &dlgEntry : dlgEntries) {
            auto index = dlgEntry->getInt("Index");
            entries.push_back(ReplyEntryLink {index});
        }

        auto reply = Reply();
        reply.text = move(text);
        reply.entries = move(entries);
        _replies.push_back(move(reply));
    }
}

void Dialog::pickReply(int index) {
    if (isOutOfRange(_currentReplies, index)) {
        throw ArgumentException(str(boost::format("Reply out of range: %d/%llu") % index % _currentReplies.size()));
    }
    auto &reply = _currentReplies[index];

    _currentEntries.swap(queue<Entry *>());
    for (auto &entryLink : reply->entries) {
        if (isOutOfRange(_entries, entryLink.index)) {
            throw ValidationException(str(boost::format("Entry out of range: %d/%llu") % entryLink.index % _entries.size()));
        }
        auto &entry = _entries[entryLink.index];
        _currentEntries.push(&entry);
    }
}

} // namespace game

} // namespace reone
