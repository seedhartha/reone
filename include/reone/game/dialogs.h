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

#include "reone/common/memorycache.h"

#include "dialog.h"

namespace reone {

namespace resource {

class Gffs;
class Gff;
class Strings;

} // namespace resource

namespace game {

class Dialogs : public MemoryCache<std::string, Dialog> {
public:
    Dialogs(resource::Gffs &gffs, resource::Strings &strings) :
        MemoryCache(std::bind(&Dialogs::doGet, this, std::placeholders::_1)),
        _gffs(gffs),
        _strings(strings) {
    }

private:
    resource::Gffs &_gffs;
    resource::Strings &_strings;

    std::shared_ptr<Dialog> doGet(std::string resRef);

    std::unique_ptr<Dialog> loadDialog(const resource::Gff &dlg);

    Dialog::EntryReplyLink getEntryReplyLink(const resource::Gff &gffs) const;
    Dialog::EntryReply getEntryReply(const resource::Gff &gffs) const;
    Dialog::Stunt getStunt(const resource::Gff &gffs) const;
    Dialog::ParticipantAnimation getParticipantAnimation(const resource::Gff &gffs) const;
};

} // namespace game

} // namespace reone
