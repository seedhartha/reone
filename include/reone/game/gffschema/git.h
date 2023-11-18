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

namespace gffschema {

struct GIT_TriggerList_Geometry {
    float PointX {0.0f};
    float PointY {0.0f};
    float PointZ {0.0f};
};

struct GIT_Encounter_List_SpawnPointList {
    float Orientation {0.0f};
    float X {0.0f};
    float Y {0.0f};
    float Z {0.0f};
};

struct GIT_Encounter_List_Geometry {
    float X {0.0f};
    float Y {0.0f};
    float Z {0.0f};
};

struct GIT_WaypointList {
    uint8_t Appearance {0};
    std::pair<int, std::string> Description;
    uint8_t HasMapNote {0};
    std::string LinkedTo;
    std::pair<int, std::string> LocalizedName;
    std::pair<int, std::string> MapNote;
    uint8_t MapNoteEnabled {0};
    std::string Tag;
    std::string TemplateResRef;
    float XOrientation {0.0f};
    float XPosition {0.0f};
    float YOrientation {0.0f};
    float YPosition {0.0f};
    float ZPosition {0.0f};
};

struct GIT_TriggerList {
    std::vector<GIT_TriggerList_Geometry> Geometry;
    std::string LinkedTo;
    uint8_t LinkedToFlags {0};
    std::string LinkedToModule;
    std::string Tag;
    std::string TemplateResRef;
    std::pair<int, std::string> TransitionDestin;
    float XOrientation {0.0f};
    float XPosition {0.0f};
    float YOrientation {0.0f};
    float YPosition {0.0f};
    float ZOrientation {0.0f};
    float ZPosition {0.0f};
};

struct GIT_StoreList {
    std::string ResRef;
    float XOrientation {0.0f};
    float XPosition {0.0f};
    float YOrientation {0.0f};
    float YPosition {0.0f};
    float ZPosition {0.0f};
};

struct GIT_SoundList {
    uint32_t GeneratedType {0};
    std::string TemplateResRef;
    float XPosition {0.0f};
    float YPosition {0.0f};
    float ZPosition {0.0f};
};

struct GIT_Placeable_List {
    float Bearing {0.0f};
    std::string TemplateResRef;
    uint32_t TweakColor {0};
    uint8_t UseTweakColor {0};
    float X {0.0f};
    float Y {0.0f};
    float Z {0.0f};
};

struct GIT_Encounter_List {
    std::vector<GIT_Encounter_List_Geometry> Geometry;
    std::vector<GIT_Encounter_List_SpawnPointList> SpawnPointList;
    std::string TemplateResRef;
    float XPosition {0.0f};
    float YPosition {0.0f};
    float ZPosition {0.0f};
};

struct GIT_Door_List {
    float Bearing {0.0f};
    std::string LinkedTo;
    uint8_t LinkedToFlags {0};
    std::string LinkedToModule;
    std::string Tag;
    std::string TemplateResRef;
    std::pair<int, std::string> TransitionDestin;
    uint32_t TweakColor {0};
    uint8_t UseTweakColor {0};
    float X {0.0f};
    float Y {0.0f};
    float Z {0.0f};
};

struct GIT_Creature_List {
    std::string TemplateResRef;
    float XOrientation {0.0f};
    float XPosition {0.0f};
    float YOrientation {0.0f};
    float YPosition {0.0f};
    float ZPosition {0.0f};
};

struct GIT_CameraList {
    int CameraID {0};
    float FieldOfView {0.0f};
    float Height {0.0f};
    float MicRange {0.0f};
    glm::quat Orientation {1.0f, 0.0f, 0.0f, 0.0f};
    float Pitch {0.0f};
    glm::vec3 Position {0.0f};
};

struct GIT_AreaProperties {
    int AmbientSndDay {0};
    int AmbientSndDayVol {0};
    int AmbientSndNight {0};
    int AmbientSndNitVol {0};
    int EnvAudio {0};
    int MusicBattle {0};
    int MusicDay {0};
    int MusicDelay {0};
    int MusicNight {0};
};

struct GIT {
    GIT_AreaProperties AreaProperties;
    std::vector<GIT_CameraList> CameraList;
    std::vector<GIT_Creature_List> Creature_List;
    std::vector<GIT_Door_List> Door_List;
    std::vector<GIT_Encounter_List> Encounter_List;
    std::vector<void *> List;
    std::vector<GIT_Placeable_List> Placeable_List;
    std::vector<GIT_SoundList> SoundList;
    std::vector<GIT_StoreList> StoreList;
    std::vector<GIT_TriggerList> TriggerList;
    uint8_t UseTemplates {0};
    std::vector<GIT_WaypointList> WaypointList;
};

GIT parseGIT(const resource::Gff &gff);

} // namespace gffschema

} // namespace game

} // namespace reone
