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

#pragma once

namespace reone {

namespace resource {

class Gff;

}

namespace game {

namespace generated {

struct DLG_EntryReplyList_EntriesRepliesList {
    std::string Active;
    std::string Active2;
    uint32_t Index {0};
    uint8_t IsChild {0};
    std::string LinkComment;
    int Logic {0};
    uint8_t Not {0};
    uint8_t Not2 {0};
    int Param1 {0};
    int Param1b {0};
    int Param2 {0};
    int Param2b {0};
    int Param3 {0};
    int Param3b {0};
    int Param4 {0};
    int Param4b {0};
    int Param5 {0};
    int Param5b {0};
    std::string ParamStrA;
    std::string ParamStrB;
};

struct DLG_EntryReplyList_AnimList {
    uint16_t Animation {0};
    std::string Participant;
};

struct DLG_StuntList {
    std::string Participant;
    std::string StuntModel;
};

struct DLG_EntryReplyList {
    int ActionParam1 {0};
    int ActionParam1b {0};
    int ActionParam2 {0};
    int ActionParam2b {0};
    int ActionParam3 {0};
    int ActionParam3b {0};
    int ActionParam4 {0};
    int ActionParam4b {0};
    int ActionParam5 {0};
    int ActionParam5b {0};
    std::string ActionParamStrA;
    std::string ActionParamStrB;
    int AlienRaceNode {0};
    std::vector<DLG_EntryReplyList_AnimList> AnimList;
    float CamFieldOfView {0.0f};
    float CamHeightOffset {0.0f};
    int CamVidEffect {0};
    uint32_t CameraAngle {0};
    uint16_t CameraAnimation {0};
    int CameraID {0};
    uint8_t Changed {0};
    std::string Comment;
    uint32_t Delay {0};
    int Emotion {0};
    std::vector<DLG_EntryReplyList_EntriesRepliesList> EntriesList;
    int FacialAnim {0};
    glm::vec3 FadeColor {0.0f};
    float FadeDelay {0.0f};
    float FadeLength {0.0f};
    uint8_t FadeType {0};
    std::string Listener;
    int NodeID {0};
    int NodeUnskippable {0};
    int PlotIndex {0};
    float PlotXPPercentage {0.0f};
    int PostProcNode {0};
    std::string Quest;
    uint32_t QuestEntry {0};
    int RecordNoVOOverri {0};
    int RecordVO {0};
    std::vector<DLG_EntryReplyList_EntriesRepliesList> RepliesList;
    std::string Script;
    std::string Script2;
    std::string Sound;
    uint8_t SoundExists {0};
    std::string Speaker;
    float TarHeightOffset {0.0f};
    std::pair<int, std::string> Text;
    uint8_t VOTextChanged {0};
    std::string VO_ResRef;
    uint32_t WaitFlags {0};
};

struct DLG {
    int AlienRaceOwner {0};
    std::string AmbientTrack;
    uint8_t AnimatedCut {0};
    std::string CameraModel;
    uint8_t ComputerType {0};
    int ConversationType {0};
    uint32_t DelayEntry {0};
    uint32_t DelayReply {0};
    std::string DeletedVOFiles;
    std::string EditorInfo;
    std::string EndConverAbort;
    std::string EndConversation;
    std::vector<DLG_EntryReplyList> EntryList;
    int NextNodeID {0};
    uint32_t NumWords {0};
    uint8_t OldHitCheck {0};
    int PostProcOwner {0};
    int RecordNoVO {0};
    std::vector<DLG_EntryReplyList> ReplyList;
    uint8_t Skippable {0};
    std::vector<DLG_EntryReplyList_EntriesRepliesList> StartingList;
    std::vector<DLG_StuntList> StuntList;
    uint8_t UnequipHItem {0};
    uint8_t UnequipItems {0};
    std::string VO_ID;
};

DLG parseDLG(const resource::Gff &gff);

} // namespace generated

} // namespace game

} // namespace reone
