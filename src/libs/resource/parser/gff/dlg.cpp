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

#include "reone/resource/parser/gff/dlg.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

namespace generated {

static DLG_EntryReplyList_EntriesRepliesList parseDLG_EntryReplyList_EntriesRepliesList(const Gff &gff) {
    DLG_EntryReplyList_EntriesRepliesList strct;
    strct.Active = gff.getString("Active");
    strct.Active2 = gff.getString("Active2");
    strct.Index = gff.getUint("Index");
    strct.IsChild = gff.getUint("IsChild");
    strct.LinkComment = gff.getString("LinkComment");
    strct.Logic = gff.getInt("Logic");
    strct.Not = gff.getUint("Not");
    strct.Not2 = gff.getUint("Not2");
    strct.Param1 = gff.getInt("Param1");
    strct.Param1b = gff.getInt("Param1b");
    strct.Param2 = gff.getInt("Param2");
    strct.Param2b = gff.getInt("Param2b");
    strct.Param3 = gff.getInt("Param3");
    strct.Param3b = gff.getInt("Param3b");
    strct.Param4 = gff.getInt("Param4");
    strct.Param4b = gff.getInt("Param4b");
    strct.Param5 = gff.getInt("Param5");
    strct.Param5b = gff.getInt("Param5b");
    strct.ParamStrA = gff.getString("ParamStrA");
    strct.ParamStrB = gff.getString("ParamStrB");
    return strct;
}

static DLG_EntryReplyList_AnimList parseDLG_EntryReplyList_AnimList(const Gff &gff) {
    DLG_EntryReplyList_AnimList strct;
    strct.Animation = gff.getUint("Animation");
    strct.Participant = gff.getString("Participant");
    return strct;
}

static DLG_StuntList parseDLG_StuntList(const Gff &gff) {
    DLG_StuntList strct;
    strct.Participant = gff.getString("Participant");
    strct.StuntModel = gff.getString("StuntModel");
    return strct;
}

static DLG_EntryReplyList parseDLG_EntryReplyList(const Gff &gff) {
    DLG_EntryReplyList strct;
    strct.ActionParam1 = gff.getInt("ActionParam1");
    strct.ActionParam1b = gff.getInt("ActionParam1b");
    strct.ActionParam2 = gff.getInt("ActionParam2");
    strct.ActionParam2b = gff.getInt("ActionParam2b");
    strct.ActionParam3 = gff.getInt("ActionParam3");
    strct.ActionParam3b = gff.getInt("ActionParam3b");
    strct.ActionParam4 = gff.getInt("ActionParam4");
    strct.ActionParam4b = gff.getInt("ActionParam4b");
    strct.ActionParam5 = gff.getInt("ActionParam5");
    strct.ActionParam5b = gff.getInt("ActionParam5b");
    strct.ActionParamStrA = gff.getString("ActionParamStrA");
    strct.ActionParamStrB = gff.getString("ActionParamStrB");
    strct.AlienRaceNode = gff.getInt("AlienRaceNode");
    for (auto &item : gff.getList("AnimList")) {
        strct.AnimList.push_back(parseDLG_EntryReplyList_AnimList(*item));
    }
    strct.CamFieldOfView = gff.getFloat("CamFieldOfView");
    strct.CamHeightOffset = gff.getFloat("CamHeightOffset");
    strct.CamVidEffect = gff.getInt("CamVidEffect");
    strct.CameraAngle = gff.getUint("CameraAngle");
    strct.CameraAnimation = gff.getUint("CameraAnimation");
    strct.CameraID = gff.getInt("CameraID");
    strct.Changed = gff.getUint("Changed");
    strct.Comment = gff.getString("Comment");
    strct.Delay = gff.getUint("Delay");
    strct.Emotion = gff.getInt("Emotion");
    for (auto &item : gff.getList("EntriesList")) {
        strct.EntriesList.push_back(parseDLG_EntryReplyList_EntriesRepliesList(*item));
    }
    strct.FacialAnim = gff.getInt("FacialAnim");
    strct.FadeColor = gff.getVector("FadeColor");
    strct.FadeDelay = gff.getFloat("FadeDelay");
    strct.FadeLength = gff.getFloat("FadeLength");
    strct.FadeType = gff.getUint("FadeType");
    strct.Listener = gff.getString("Listener");
    strct.NodeID = gff.getInt("NodeID");
    strct.NodeUnskippable = gff.getInt("NodeUnskippable");
    strct.PlotIndex = gff.getInt("PlotIndex");
    strct.PlotXPPercentage = gff.getFloat("PlotXPPercentage");
    strct.PostProcNode = gff.getInt("PostProcNode");
    strct.Quest = gff.getString("Quest");
    strct.QuestEntry = gff.getUint("QuestEntry");
    strct.RecordNoVOOverri = gff.getInt("RecordNoVOOverri");
    strct.RecordVO = gff.getInt("RecordVO");
    for (auto &item : gff.getList("RepliesList")) {
        strct.RepliesList.push_back(parseDLG_EntryReplyList_EntriesRepliesList(*item));
    }
    strct.Script = gff.getString("Script");
    strct.Script2 = gff.getString("Script2");
    strct.Sound = gff.getString("Sound");
    strct.SoundExists = gff.getUint("SoundExists");
    strct.Speaker = gff.getString("Speaker");
    strct.TarHeightOffset = gff.getFloat("TarHeightOffset");
    strct.Text = std::make_pair(gff.getInt("Text"), gff.getString("Text"));
    strct.VOTextChanged = gff.getUint("VOTextChanged");
    strct.VO_ResRef = gff.getString("VO_ResRef");
    strct.WaitFlags = gff.getUint("WaitFlags");
    return strct;
}

DLG parseDLG(const Gff &gff) {
    DLG strct;
    strct.AlienRaceOwner = gff.getInt("AlienRaceOwner");
    strct.AmbientTrack = gff.getString("AmbientTrack");
    strct.AnimatedCut = gff.getUint("AnimatedCut");
    strct.CameraModel = gff.getString("CameraModel");
    strct.ComputerType = gff.getUint("ComputerType");
    strct.ConversationType = gff.getInt("ConversationType");
    strct.DelayEntry = gff.getUint("DelayEntry");
    strct.DelayReply = gff.getUint("DelayReply");
    strct.DeletedVOFiles = gff.getString("DeletedVOFiles");
    strct.EditorInfo = gff.getString("EditorInfo");
    strct.EndConverAbort = gff.getString("EndConverAbort");
    strct.EndConversation = gff.getString("EndConversation");
    for (auto &item : gff.getList("EntryList")) {
        strct.EntryList.push_back(parseDLG_EntryReplyList(*item));
    }
    strct.NextNodeID = gff.getInt("NextNodeID");
    strct.NumWords = gff.getUint("NumWords");
    strct.OldHitCheck = gff.getUint("OldHitCheck");
    strct.PostProcOwner = gff.getInt("PostProcOwner");
    strct.RecordNoVO = gff.getInt("RecordNoVO");
    for (auto &item : gff.getList("ReplyList")) {
        strct.ReplyList.push_back(parseDLG_EntryReplyList(*item));
    }
    strct.Skippable = gff.getUint("Skippable");
    for (auto &item : gff.getList("StartingList")) {
        strct.StartingList.push_back(parseDLG_EntryReplyList_EntriesRepliesList(*item));
    }
    for (auto &item : gff.getList("StuntList")) {
        strct.StuntList.push_back(parseDLG_StuntList(*item));
    }
    strct.UnequipHItem = gff.getUint("UnequipHItem");
    strct.UnequipItems = gff.getUint("UnequipItems");
    strct.VO_ID = gff.getString("VO_ID");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
