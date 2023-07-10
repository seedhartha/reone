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

#include "reone/game/schema/git.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace schema {

static GIT_TriggerList_Geometry parseGIT_TriggerList_Geometry(const Gff &gff) {
    GIT_TriggerList_Geometry strct;
    strct.PointX = gff.getFloat("PointX");
    strct.PointY = gff.getFloat("PointY");
    strct.PointZ = gff.getFloat("PointZ");
    return strct;
}

static GIT_Encounter_List_SpawnPointList parseGIT_Encounter_List_SpawnPointList(const Gff &gff) {
    GIT_Encounter_List_SpawnPointList strct;
    strct.Orientation = gff.getFloat("Orientation");
    strct.X = gff.getFloat("X");
    strct.Y = gff.getFloat("Y");
    strct.Z = gff.getFloat("Z");
    return strct;
}

static GIT_Encounter_List_Geometry parseGIT_Encounter_List_Geometry(const Gff &gff) {
    GIT_Encounter_List_Geometry strct;
    strct.X = gff.getFloat("X");
    strct.Y = gff.getFloat("Y");
    strct.Z = gff.getFloat("Z");
    return strct;
}

static GIT_WaypointList parseGIT_WaypointList(const Gff &gff) {
    GIT_WaypointList strct;
    strct.Appearance = gff.getUint("Appearance");
    strct.Description = std::make_pair(gff.getInt("Description"), gff.getString("Description"));
    strct.HasMapNote = gff.getUint("HasMapNote");
    strct.LinkedTo = gff.getString("LinkedTo");
    strct.LocalizedName = std::make_pair(gff.getInt("LocalizedName"), gff.getString("LocalizedName"));
    strct.MapNote = std::make_pair(gff.getInt("MapNote"), gff.getString("MapNote"));
    strct.MapNoteEnabled = gff.getUint("MapNoteEnabled");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.XOrientation = gff.getFloat("XOrientation");
    strct.XPosition = gff.getFloat("XPosition");
    strct.YOrientation = gff.getFloat("YOrientation");
    strct.YPosition = gff.getFloat("YPosition");
    strct.ZPosition = gff.getFloat("ZPosition");
    return strct;
}

static GIT_TriggerList parseGIT_TriggerList(const Gff &gff) {
    GIT_TriggerList strct;
    for (auto &item : gff.getList("Geometry")) {
        strct.Geometry.push_back(parseGIT_TriggerList_Geometry(*item));
    }
    strct.LinkedTo = gff.getString("LinkedTo");
    strct.LinkedToFlags = gff.getUint("LinkedToFlags");
    strct.LinkedToModule = gff.getString("LinkedToModule");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TransitionDestin = std::make_pair(gff.getInt("TransitionDestin"), gff.getString("TransitionDestin"));
    strct.XOrientation = gff.getFloat("XOrientation");
    strct.XPosition = gff.getFloat("XPosition");
    strct.YOrientation = gff.getFloat("YOrientation");
    strct.YPosition = gff.getFloat("YPosition");
    strct.ZOrientation = gff.getFloat("ZOrientation");
    strct.ZPosition = gff.getFloat("ZPosition");
    return strct;
}

static GIT_StoreList parseGIT_StoreList(const Gff &gff) {
    GIT_StoreList strct;
    strct.ResRef = gff.getString("ResRef");
    strct.XOrientation = gff.getFloat("XOrientation");
    strct.XPosition = gff.getFloat("XPosition");
    strct.YOrientation = gff.getFloat("YOrientation");
    strct.YPosition = gff.getFloat("YPosition");
    strct.ZPosition = gff.getFloat("ZPosition");
    return strct;
}

static GIT_SoundList parseGIT_SoundList(const Gff &gff) {
    GIT_SoundList strct;
    strct.GeneratedType = gff.getUint("GeneratedType");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.XPosition = gff.getFloat("XPosition");
    strct.YPosition = gff.getFloat("YPosition");
    strct.ZPosition = gff.getFloat("ZPosition");
    return strct;
}

static GIT_Placeable_List parseGIT_Placeable_List(const Gff &gff) {
    GIT_Placeable_List strct;
    strct.Bearing = gff.getFloat("Bearing");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TweakColor = gff.getUint("TweakColor");
    strct.UseTweakColor = gff.getUint("UseTweakColor");
    strct.X = gff.getFloat("X");
    strct.Y = gff.getFloat("Y");
    strct.Z = gff.getFloat("Z");
    return strct;
}

static GIT_Encounter_List parseGIT_Encounter_List(const Gff &gff) {
    GIT_Encounter_List strct;
    for (auto &item : gff.getList("Geometry")) {
        strct.Geometry.push_back(parseGIT_Encounter_List_Geometry(*item));
    }
    for (auto &item : gff.getList("SpawnPointList")) {
        strct.SpawnPointList.push_back(parseGIT_Encounter_List_SpawnPointList(*item));
    }
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.XPosition = gff.getFloat("XPosition");
    strct.YPosition = gff.getFloat("YPosition");
    strct.ZPosition = gff.getFloat("ZPosition");
    return strct;
}

static GIT_Door_List parseGIT_Door_List(const Gff &gff) {
    GIT_Door_List strct;
    strct.Bearing = gff.getFloat("Bearing");
    strct.LinkedTo = gff.getString("LinkedTo");
    strct.LinkedToFlags = gff.getUint("LinkedToFlags");
    strct.LinkedToModule = gff.getString("LinkedToModule");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TransitionDestin = std::make_pair(gff.getInt("TransitionDestin"), gff.getString("TransitionDestin"));
    strct.TweakColor = gff.getUint("TweakColor");
    strct.UseTweakColor = gff.getUint("UseTweakColor");
    strct.X = gff.getFloat("X");
    strct.Y = gff.getFloat("Y");
    strct.Z = gff.getFloat("Z");
    return strct;
}

static GIT_Creature_List parseGIT_Creature_List(const Gff &gff) {
    GIT_Creature_List strct;
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.XOrientation = gff.getFloat("XOrientation");
    strct.XPosition = gff.getFloat("XPosition");
    strct.YOrientation = gff.getFloat("YOrientation");
    strct.YPosition = gff.getFloat("YPosition");
    strct.ZPosition = gff.getFloat("ZPosition");
    return strct;
}

static GIT_CameraList parseGIT_CameraList(const Gff &gff) {
    GIT_CameraList strct;
    strct.CameraID = gff.getInt("CameraID");
    strct.FieldOfView = gff.getFloat("FieldOfView");
    strct.Height = gff.getFloat("Height");
    strct.MicRange = gff.getFloat("MicRange");
    strct.Orientation = gff.getOrientation("Orientation");
    strct.Pitch = gff.getFloat("Pitch");
    strct.Position = gff.getVector("Position");
    return strct;
}

static GIT_AreaProperties parseGIT_AreaProperties(const Gff &gff) {
    GIT_AreaProperties strct;
    strct.AmbientSndDay = gff.getInt("AmbientSndDay");
    strct.AmbientSndDayVol = gff.getInt("AmbientSndDayVol");
    strct.AmbientSndNight = gff.getInt("AmbientSndNight");
    strct.AmbientSndNitVol = gff.getInt("AmbientSndNitVol");
    strct.EnvAudio = gff.getInt("EnvAudio");
    strct.MusicBattle = gff.getInt("MusicBattle");
    strct.MusicDay = gff.getInt("MusicDay");
    strct.MusicDelay = gff.getInt("MusicDelay");
    strct.MusicNight = gff.getInt("MusicNight");
    return strct;
}

GIT parseGIT(const Gff &gff) {
    GIT strct;
    auto AreaProperties = gff.findStruct("AreaProperties");
    if (AreaProperties) {
        strct.AreaProperties = parseGIT_AreaProperties(*AreaProperties);
    }
    for (auto &item : gff.getList("CameraList")) {
        strct.CameraList.push_back(parseGIT_CameraList(*item));
    }
    for (auto &item : gff.getList("Creature List")) {
        strct.Creature_List.push_back(parseGIT_Creature_List(*item));
    }
    for (auto &item : gff.getList("Door List")) {
        strct.Door_List.push_back(parseGIT_Door_List(*item));
    }
    for (auto &item : gff.getList("Encounter List")) {
        strct.Encounter_List.push_back(parseGIT_Encounter_List(*item));
    }
    for (auto &item : gff.getList("Placeable List")) {
        strct.Placeable_List.push_back(parseGIT_Placeable_List(*item));
    }
    for (auto &item : gff.getList("SoundList")) {
        strct.SoundList.push_back(parseGIT_SoundList(*item));
    }
    for (auto &item : gff.getList("StoreList")) {
        strct.StoreList.push_back(parseGIT_StoreList(*item));
    }
    for (auto &item : gff.getList("TriggerList")) {
        strct.TriggerList.push_back(parseGIT_TriggerList(*item));
    }
    strct.UseTemplates = gff.getUint("UseTemplates");
    for (auto &item : gff.getList("WaypointList")) {
        strct.WaypointList.push_back(parseGIT_WaypointList(*item));
    }
    return strct;
}

} // namespace schema

} // namespace game

} // namespace reone
