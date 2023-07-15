/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/dialogs.h"

#include "reone/resource/gffs.h"
#include "reone/resource/strings.h"

using namespace reone::resource;

namespace reone {

namespace game {

std::shared_ptr<Dialog> Dialogs::doGet(std::string resRef) {
    auto dlg = _gffs.get(resRef, ResourceType::Dlg);
    if (!dlg) {
        return nullptr;
    }
    auto dlgParsed = schema::parseDLG(*dlg);
    return loadDialog(dlgParsed);
}

std::unique_ptr<Dialog> Dialogs::loadDialog(const schema::DLG &dlg) {
    auto dialog = std::make_unique<Dialog>();

    dialog->skippable = dlg.Skippable;
    dialog->cameraModel = dlg.CameraModel;
    dialog->endScript = dlg.EndConversation;
    dialog->animatedCutscene = dlg.AnimatedCut;
    dialog->conversationType = static_cast<ConversationType>(dlg.ConversationType);
    dialog->computerType = static_cast<ComputerType>(dlg.ComputerType);

    for (auto &entry : dlg.EntryList) {
        dialog->entries.push_back(getEntryReply(entry));
    }
    for (auto &reply : dlg.ReplyList) {
        dialog->replies.push_back(getEntryReply(reply));
    }
    for (auto &entry : dlg.StartingList) {
        dialog->startEntries.push_back(getEntryReplyLink(entry));
    }
    for (auto &stunt : dlg.StuntList) {
        dialog->stunts.push_back(getStunt(stunt));
    }

    return std::move(dialog);
}

Dialog::EntryReplyLink Dialogs::getEntryReplyLink(const schema::DLG_EntryReplyList_EntriesRepliesList &dlg) const {
    Dialog::EntryReplyLink link;
    link.index = dlg.Index;
    link.active = dlg.Active;

    return std::move(link);
}

Dialog::EntryReply Dialogs::getEntryReply(const schema::DLG_EntryReplyList &dlg) const {
    int strRef = dlg.Text.first;

    Dialog::EntryReply entry;
    entry.speaker = dlg.Speaker;
    entry.text = strRef == -1 ? "" : _strings.getText(strRef);
    entry.voResRef = dlg.VO_ResRef;
    entry.script = dlg.Script;
    entry.sound = dlg.Sound;
    entry.listener = dlg.Listener;
    entry.delay = dlg.Delay;
    entry.waitFlags = dlg.WaitFlags;
    entry.cameraId = dlg.CameraID;
    entry.cameraAngle = dlg.CameraAngle;
    entry.cameraAnimation = dlg.CameraAnimation;
    entry.camFieldOfView = dlg.CamFieldOfView;

    boost::to_lower(entry.speaker);
    boost::to_lower(entry.listener);

    for (auto &link : dlg.RepliesList) {
        entry.replies.push_back(getEntryReplyLink(link));
    }
    for (auto &link : dlg.EntriesList) {
        entry.entries.push_back(getEntryReplyLink(link));
    }
    for (auto &anim : dlg.AnimList) {
        entry.animations.push_back(getParticipantAnimation(anim));
    }

    return std::move(entry);
}

Dialog::Stunt Dialogs::getStunt(const schema::DLG_StuntList &dlg) const {
    Dialog::Stunt stunt;
    stunt.participant = boost::to_lower_copy(dlg.Participant);
    stunt.stuntModel = boost::to_lower_copy(dlg.StuntModel);
    return stunt;
}

Dialog::ParticipantAnimation Dialogs::getParticipantAnimation(const schema::DLG_EntryReplyList_AnimList &dlg) const {
    Dialog::ParticipantAnimation anim;
    anim.participant = boost::to_lower_copy(dlg.Participant);
    anim.animation = dlg.Animation;
    return std::move(anim);
}

} // namespace game

} // namespace reone
