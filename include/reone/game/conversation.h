/*
 * Copyright (c) 2020-2022 The reone project contributors
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

namespace reone {

namespace resource {

struct ResourceServices;

class Gff;

} // namespace resource

namespace game {

class Conversation : boost::noncopyable {
public:
    struct EntryReplyLink {
        int index {0};
    };

    struct Entry {
        std::string text;
        std::vector<EntryReplyLink> replies;
    };

    struct ReplyEntryLink {
        int index {0};
    };

    struct Reply {
        std::string text;
        std::vector<ReplyEntryLink> entries;
    };

    Conversation(resource::ResourceServices &resourceSvc) :
        _resourceSvc(resourceSvc) {
    }

    void load(const std::string &name);

    void pickReply(int index);

    const std::queue<Entry *> &currentEntries() const {
        return _currentEntries;
    }

    const std::vector<Reply *> &currentReplies() const {
        return _currentReplies;
    }

    const std::vector<Entry> &entries() const {
        return _entries;
    }

    const std::vector<Reply> &replies() const {
        return _replies;
    }

private:
    resource::ResourceServices &_resourceSvc;

    std::vector<Entry> _entries;
    std::vector<Reply> _replies;

    std::queue<Entry *> _currentEntries;
    std::vector<Reply *> _currentReplies;

    void loadEntries(const resource::Gff &dlg);
    void loadReplies(const resource::Gff &dlg);
};

} // namespace game

} // namespace reone
