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

#include "reone/game/generated/are.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace generated {

static ARE_MiniGame_Player_Gun_Banks_Bullet parseARE_MiniGame_Player_Gun_Banks_Bullet(const Gff &gff) {
    ARE_MiniGame_Player_Gun_Banks_Bullet strct;
    strct.Bullet_Model = gff.getString("Bullet_Model");
    strct.Collision_Sound = gff.getString("Collision_Sound");
    strct.Damage = gff.getUint("Damage");
    strct.Lifespan = gff.getFloat("Lifespan");
    strct.Rate_Of_Fire = gff.getFloat("Rate_Of_Fire");
    strct.Speed = gff.getFloat("Speed");
    strct.Target_Type = gff.getUint("Target_Type");
    return strct;
}

static ARE_MiniGame_Enemies_Gun_Banks_Bullet parseARE_MiniGame_Enemies_Gun_Banks_Bullet(const Gff &gff) {
    ARE_MiniGame_Enemies_Gun_Banks_Bullet strct;
    strct.Bullet_Model = gff.getString("Bullet_Model");
    strct.Collision_Sound = gff.getString("Collision_Sound");
    strct.Damage = gff.getUint("Damage");
    strct.Lifespan = gff.getFloat("Lifespan");
    strct.Rate_Of_Fire = gff.getFloat("Rate_Of_Fire");
    strct.Speed = gff.getFloat("Speed");
    strct.Target_Type = gff.getUint("Target_Type");
    return strct;
}

static ARE_MiniGame_Player_Sounds parseARE_MiniGame_Player_Sounds(const Gff &gff) {
    ARE_MiniGame_Player_Sounds strct;
    strct.Death = gff.getString("Death");
    strct.Engine = gff.getString("Engine");
    return strct;
}

static ARE_MiniGame_Player_Scripts parseARE_MiniGame_Player_Scripts(const Gff &gff) {
    ARE_MiniGame_Player_Scripts strct;
    strct.OnAccelerate = gff.getString("OnAccelerate");
    strct.OnAnimEvent = gff.getString("OnAnimEvent");
    strct.OnBrake = gff.getString("OnBrake");
    strct.OnCreate = gff.getString("OnCreate");
    strct.OnDamage = gff.getString("OnDamage");
    strct.OnDeath = gff.getString("OnDeath");
    strct.OnFire = gff.getString("OnFire");
    strct.OnHeartbeat = gff.getString("OnHeartbeat");
    strct.OnHitBullet = gff.getString("OnHitBullet");
    strct.OnHitFollower = gff.getString("OnHitFollower");
    strct.OnHitObstacle = gff.getString("OnHitObstacle");
    strct.OnHitWorld = gff.getString("OnHitWorld");
    strct.OnTrackLoop = gff.getString("OnTrackLoop");
    return strct;
}

static ARE_MiniGame_Player_Models parseARE_MiniGame_Player_Models(const Gff &gff) {
    ARE_MiniGame_Player_Models strct;
    strct.Model = gff.getString("Model");
    strct.RotatingModel = gff.getUint("RotatingModel");
    return strct;
}

static ARE_MiniGame_Player_Gun_Banks parseARE_MiniGame_Player_Gun_Banks(const Gff &gff) {
    ARE_MiniGame_Player_Gun_Banks strct;
    strct.BankID = gff.getUint("BankID");
    auto Bullet = gff.findStruct("Bullet");
    if (Bullet) {
        strct.Bullet = parseARE_MiniGame_Player_Gun_Banks_Bullet(*Bullet);
    }
    strct.Fire_Sound = gff.getString("Fire_Sound");
    strct.Gun_Model = gff.getString("Gun_Model");
    return strct;
}

static ARE_MiniGame_Obstacles_Scripts parseARE_MiniGame_Obstacles_Scripts(const Gff &gff) {
    ARE_MiniGame_Obstacles_Scripts strct;
    strct.OnAnimEvent = gff.getString("OnAnimEvent");
    strct.OnCreate = gff.getString("OnCreate");
    strct.OnHeartbeat = gff.getString("OnHeartbeat");
    strct.OnHitBullet = gff.getString("OnHitBullet");
    strct.OnHitFollower = gff.getString("OnHitFollower");
    return strct;
}

static ARE_MiniGame_Enemies_Sounds parseARE_MiniGame_Enemies_Sounds(const Gff &gff) {
    ARE_MiniGame_Enemies_Sounds strct;
    strct.Death = gff.getString("Death");
    strct.Engine = gff.getString("Engine");
    return strct;
}

static ARE_MiniGame_Enemies_Scripts parseARE_MiniGame_Enemies_Scripts(const Gff &gff) {
    ARE_MiniGame_Enemies_Scripts strct;
    strct.OnAccelerate = gff.getString("OnAccelerate");
    strct.OnAnimEvent = gff.getString("OnAnimEvent");
    strct.OnBrake = gff.getString("OnBrake");
    strct.OnCreate = gff.getString("OnCreate");
    strct.OnDamage = gff.getString("OnDamage");
    strct.OnDeath = gff.getString("OnDeath");
    strct.OnFire = gff.getString("OnFire");
    strct.OnHeartbeat = gff.getString("OnHeartbeat");
    strct.OnHitBullet = gff.getString("OnHitBullet");
    strct.OnHitFollower = gff.getString("OnHitFollower");
    strct.OnHitObstacle = gff.getString("OnHitObstacle");
    strct.OnHitWorld = gff.getString("OnHitWorld");
    strct.OnTrackLoop = gff.getString("OnTrackLoop");
    return strct;
}

static ARE_MiniGame_Enemies_Models parseARE_MiniGame_Enemies_Models(const Gff &gff) {
    ARE_MiniGame_Enemies_Models strct;
    strct.Model = gff.getString("Model");
    strct.RotatingModel = gff.getUint("RotatingModel");
    return strct;
}

static ARE_MiniGame_Enemies_Gun_Banks parseARE_MiniGame_Enemies_Gun_Banks(const Gff &gff) {
    ARE_MiniGame_Enemies_Gun_Banks strct;
    strct.BankID = gff.getUint("BankID");
    auto Bullet = gff.findStruct("Bullet");
    if (Bullet) {
        strct.Bullet = parseARE_MiniGame_Enemies_Gun_Banks_Bullet(*Bullet);
    }
    strct.Fire_Sound = gff.getString("Fire_Sound");
    strct.Gun_Model = gff.getString("Gun_Model");
    strct.Horiz_Spread = gff.getFloat("Horiz_Spread");
    strct.Inaccuracy = gff.getFloat("Inaccuracy");
    strct.Sensing_Radius = gff.getFloat("Sensing_Radius");
    strct.Vert_Spread = gff.getFloat("Vert_Spread");
    return strct;
}

static ARE_MiniGame_Player parseARE_MiniGame_Player(const Gff &gff) {
    ARE_MiniGame_Player strct;
    strct.Accel_Secs = gff.getFloat("Accel_Secs");
    strct.Bump_Damage = gff.getInt("Bump_Damage");
    strct.Camera = gff.getString("Camera");
    strct.CameraRotate = gff.getUint("CameraRotate");
    for (auto &item : gff.getList("Gun_Banks")) {
        strct.Gun_Banks.push_back(parseARE_MiniGame_Player_Gun_Banks(*item));
    }
    strct.Hit_Points = gff.getUint("Hit_Points");
    strct.Invince_Period = gff.getFloat("Invince_Period");
    strct.Max_HPs = gff.getUint("Max_HPs");
    strct.Maximum_Speed = gff.getFloat("Maximum_Speed");
    strct.Minimum_Speed = gff.getFloat("Minimum_Speed");
    for (auto &item : gff.getList("Models")) {
        strct.Models.push_back(parseARE_MiniGame_Player_Models(*item));
    }
    strct.Num_Loops = gff.getInt("Num_Loops");
    auto Scripts = gff.findStruct("Scripts");
    if (Scripts) {
        strct.Scripts = parseARE_MiniGame_Player_Scripts(*Scripts);
    }
    auto Sounds = gff.findStruct("Sounds");
    if (Sounds) {
        strct.Sounds = parseARE_MiniGame_Player_Sounds(*Sounds);
    }
    strct.Sphere_Radius = gff.getFloat("Sphere_Radius");
    strct.Start_Offset_X = gff.getFloat("Start_Offset_X");
    strct.Start_Offset_Y = gff.getFloat("Start_Offset_Y");
    strct.Start_Offset_Z = gff.getFloat("Start_Offset_Z");
    strct.Target_Offset_X = gff.getFloat("Target_Offset_X");
    strct.Target_Offset_Y = gff.getFloat("Target_Offset_Y");
    strct.Target_Offset_Z = gff.getFloat("Target_Offset_Z");
    strct.Track = gff.getString("Track");
    strct.TunnelInfinite = gff.getVector("TunnelInfinite");
    strct.TunnelXNeg = gff.getFloat("TunnelXNeg");
    strct.TunnelXPos = gff.getFloat("TunnelXPos");
    strct.TunnelYNeg = gff.getFloat("TunnelYNeg");
    strct.TunnelYPos = gff.getFloat("TunnelYPos");
    strct.TunnelZNeg = gff.getFloat("TunnelZNeg");
    strct.TunnelZPos = gff.getFloat("TunnelZPos");
    return strct;
}

static ARE_MiniGame_Obstacles parseARE_MiniGame_Obstacles(const Gff &gff) {
    ARE_MiniGame_Obstacles strct;
    strct.Name = gff.getString("Name");
    auto Scripts = gff.findStruct("Scripts");
    if (Scripts) {
        strct.Scripts = parseARE_MiniGame_Obstacles_Scripts(*Scripts);
    }
    return strct;
}

static ARE_MiniGame_Mouse parseARE_MiniGame_Mouse(const Gff &gff) {
    ARE_MiniGame_Mouse strct;
    strct.AxisX = gff.getUint("AxisX");
    strct.AxisY = gff.getUint("AxisY");
    strct.FlipAxisX = gff.getUint("FlipAxisX");
    strct.FlipAxisY = gff.getUint("FlipAxisY");
    return strct;
}

static ARE_MiniGame_Enemies parseARE_MiniGame_Enemies(const Gff &gff) {
    ARE_MiniGame_Enemies strct;
    strct.Bump_Damage = gff.getInt("Bump_Damage");
    for (auto &item : gff.getList("Gun_Banks")) {
        strct.Gun_Banks.push_back(parseARE_MiniGame_Enemies_Gun_Banks(*item));
    }
    strct.Hit_Points = gff.getUint("Hit_Points");
    strct.Invince_Period = gff.getFloat("Invince_Period");
    strct.Max_HPs = gff.getUint("Max_HPs");
    for (auto &item : gff.getList("Models")) {
        strct.Models.push_back(parseARE_MiniGame_Enemies_Models(*item));
    }
    strct.Num_Loops = gff.getInt("Num_Loops");
    auto Scripts = gff.findStruct("Scripts");
    if (Scripts) {
        strct.Scripts = parseARE_MiniGame_Enemies_Scripts(*Scripts);
    }
    auto Sounds = gff.findStruct("Sounds");
    if (Sounds) {
        strct.Sounds = parseARE_MiniGame_Enemies_Sounds(*Sounds);
    }
    strct.Sphere_Radius = gff.getFloat("Sphere_Radius");
    strct.Track = gff.getString("Track");
    strct.Trigger = gff.getUint("Trigger");
    return strct;
}

static ARE_Rooms parseARE_Rooms(const Gff &gff) {
    ARE_Rooms strct;
    strct.AmbientScale = gff.getFloat("AmbientScale");
    strct.DisableWeather = gff.getUint("DisableWeather");
    strct.EnvAudio = gff.getInt("EnvAudio");
    strct.ForceRating = gff.getInt("ForceRating");
    strct.RoomName = gff.getString("RoomName");
    return strct;
}

static ARE_MiniGame parseARE_MiniGame(const Gff &gff) {
    ARE_MiniGame strct;
    strct.Bump_Plane = gff.getUint("Bump_Plane");
    strct.CameraViewAngle = gff.getFloat("CameraViewAngle");
    strct.DOF = gff.getUint("DOF");
    strct.DoBumping = gff.getUint("DoBumping");
    for (auto &item : gff.getList("Enemies")) {
        strct.Enemies.push_back(parseARE_MiniGame_Enemies(*item));
    }
    strct.Far_Clip = gff.getFloat("Far_Clip");
    strct.LateralAccel = gff.getFloat("LateralAccel");
    auto Mouse = gff.findStruct("Mouse");
    if (Mouse) {
        strct.Mouse = parseARE_MiniGame_Mouse(*Mouse);
    }
    strct.MovementPerSec = gff.getFloat("MovementPerSec");
    strct.Music = gff.getString("Music");
    strct.Near_Clip = gff.getFloat("Near_Clip");
    for (auto &item : gff.getList("Obstacles")) {
        strct.Obstacles.push_back(parseARE_MiniGame_Obstacles(*item));
    }
    auto Player = gff.findStruct("Player");
    if (Player) {
        strct.Player = parseARE_MiniGame_Player(*Player);
    }
    strct.Type = gff.getUint("Type");
    strct.UseInertia = gff.getUint("UseInertia");
    return strct;
}

static ARE_Map parseARE_Map(const Gff &gff) {
    ARE_Map strct;
    strct.MapPt1X = gff.getFloat("MapPt1X");
    strct.MapPt1Y = gff.getFloat("MapPt1Y");
    strct.MapPt2X = gff.getFloat("MapPt2X");
    strct.MapPt2Y = gff.getFloat("MapPt2Y");
    strct.MapResX = gff.getInt("MapResX");
    strct.MapZoom = gff.getInt("MapZoom");
    strct.NorthAxis = gff.getInt("NorthAxis");
    strct.WorldPt1X = gff.getFloat("WorldPt1X");
    strct.WorldPt1Y = gff.getFloat("WorldPt1Y");
    strct.WorldPt2X = gff.getFloat("WorldPt2X");
    strct.WorldPt2Y = gff.getFloat("WorldPt2Y");
    return strct;
}

ARE parseARE(const Gff &gff) {
    ARE strct;
    strct.AlphaTest = gff.getFloat("AlphaTest");
    strct.CameraStyle = gff.getInt("CameraStyle");
    strct.ChanceLightning = gff.getInt("ChanceLightning");
    strct.ChanceRain = gff.getInt("ChanceRain");
    strct.ChanceSnow = gff.getInt("ChanceSnow");
    strct.Comments = gff.getString("Comments");
    strct.Creator_ID = gff.getInt("Creator_ID");
    strct.DayNightCycle = gff.getUint("DayNightCycle");
    strct.DefaultEnvMap = gff.getString("DefaultEnvMap");
    strct.DirtyARGBOne = gff.getInt("DirtyARGBOne");
    strct.DirtyARGBThree = gff.getInt("DirtyARGBThree");
    strct.DirtyARGBTwo = gff.getInt("DirtyARGBTwo");
    strct.DirtyFormulaOne = gff.getInt("DirtyFormulaOne");
    strct.DirtyFormulaThre = gff.getInt("DirtyFormulaThre");
    strct.DirtyFormulaTwo = gff.getInt("DirtyFormulaTwo");
    strct.DirtyFuncOne = gff.getInt("DirtyFuncOne");
    strct.DirtyFuncThree = gff.getInt("DirtyFuncThree");
    strct.DirtyFuncTwo = gff.getInt("DirtyFuncTwo");
    strct.DirtySizeOne = gff.getInt("DirtySizeOne");
    strct.DirtySizeThree = gff.getInt("DirtySizeThree");
    strct.DirtySizeTwo = gff.getInt("DirtySizeTwo");
    strct.DisableTransit = gff.getUint("DisableTransit");
    strct.DynAmbientColor = gff.getUint("DynAmbientColor");
    strct.Flags = gff.getUint("Flags");
    strct.Grass_Ambient = gff.getUint("Grass_Ambient");
    strct.Grass_Density = gff.getFloat("Grass_Density");
    strct.Grass_Diffuse = gff.getUint("Grass_Diffuse");
    strct.Grass_Emissive = gff.getUint("Grass_Emissive");
    strct.Grass_Prob_LL = gff.getFloat("Grass_Prob_LL");
    strct.Grass_Prob_LR = gff.getFloat("Grass_Prob_LR");
    strct.Grass_Prob_UL = gff.getFloat("Grass_Prob_UL");
    strct.Grass_Prob_UR = gff.getFloat("Grass_Prob_UR");
    strct.Grass_QuadSize = gff.getFloat("Grass_QuadSize");
    strct.Grass_TexName = gff.getString("Grass_TexName");
    strct.ID = gff.getInt("ID");
    strct.IsNight = gff.getUint("IsNight");
    strct.LightingScheme = gff.getUint("LightingScheme");
    strct.LoadScreenID = gff.getUint("LoadScreenID");
    auto Map = gff.findStruct("Map");
    if (Map) {
        strct.Map = parseARE_Map(*Map);
    }
    auto MiniGame = gff.findStruct("MiniGame");
    if (MiniGame) {
        strct.MiniGame = parseARE_MiniGame(*MiniGame);
    }
    strct.ModListenCheck = gff.getInt("ModListenCheck");
    strct.ModSpotCheck = gff.getInt("ModSpotCheck");
    strct.MoonAmbientColor = gff.getUint("MoonAmbientColor");
    strct.MoonDiffuseColor = gff.getUint("MoonDiffuseColor");
    strct.MoonFogColor = gff.getUint("MoonFogColor");
    strct.MoonFogFar = gff.getFloat("MoonFogFar");
    strct.MoonFogNear = gff.getFloat("MoonFogNear");
    strct.MoonFogOn = gff.getUint("MoonFogOn");
    strct.MoonShadows = gff.getUint("MoonShadows");
    strct.Name = std::make_pair(gff.getInt("Name"), gff.getString("Name"));
    strct.NoHangBack = gff.getUint("NoHangBack");
    strct.NoRest = gff.getUint("NoRest");
    strct.OnEnter = gff.getString("OnEnter");
    strct.OnExit = gff.getString("OnExit");
    strct.OnHeartbeat = gff.getString("OnHeartbeat");
    strct.OnUserDefined = gff.getString("OnUserDefined");
    strct.PlayerOnly = gff.getUint("PlayerOnly");
    strct.PlayerVsPlayer = gff.getUint("PlayerVsPlayer");
    for (auto &item : gff.getList("Rooms")) {
        strct.Rooms.push_back(parseARE_Rooms(*item));
    }
    strct.ShadowOpacity = gff.getUint("ShadowOpacity");
    strct.StealthXPEnabled = gff.getUint("StealthXPEnabled");
    strct.StealthXPLoss = gff.getUint("StealthXPLoss");
    strct.StealthXPMax = gff.getUint("StealthXPMax");
    strct.SunAmbientColor = gff.getUint("SunAmbientColor");
    strct.SunDiffuseColor = gff.getUint("SunDiffuseColor");
    strct.SunFogColor = gff.getUint("SunFogColor");
    strct.SunFogFar = gff.getFloat("SunFogFar");
    strct.SunFogNear = gff.getFloat("SunFogNear");
    strct.SunFogOn = gff.getUint("SunFogOn");
    strct.SunShadows = gff.getUint("SunShadows");
    strct.Tag = gff.getString("Tag");
    strct.Unescapable = gff.getUint("Unescapable");
    strct.Version = gff.getUint("Version");
    strct.WindPower = gff.getInt("WindPower");
    return strct;
}

} // namespace generated

} // namespace game

} // namespace reone
