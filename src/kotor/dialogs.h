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

#include "../game/dialog.h"
#include "../game/dialogs.h"

namespace reone {

namespace resource {

class Gffs;
class GffStruct;
class Strings;

} // namespace resource

namespace kotor {

class Dialogs : public game::IDialogs {
public:
    Dialogs(resource::Gffs &gffs, resource::Strings &strings) :
        IDialogs(std::bind(&Dialogs::doGet, this, std::placeholders::_1)),
        _gffs(gffs),
        _strings(strings) {
    }

private:
    resource::Gffs &_gffs;
    resource::Strings &_strings;

    std::shared_ptr<game::Dialog> doGet(std::string resRef);

    std::unique_ptr<game::Dialog> loadDialog(const resource::GffStruct &dlg);

    game::Dialog::EntryReplyLink getEntryReplyLink(const resource::GffStruct &gffs) const;
    game::Dialog::EntryReply getEntryReply(const resource::GffStruct &gffs) const;
    game::Dialog::Stunt getStunt(const resource::GffStruct &gffs) const;
    game::Dialog::ParticipantAnimation getParticipantAnimation(const resource::GffStruct &gffs) const;
};

} // namespace kotor

} // namespace reone
