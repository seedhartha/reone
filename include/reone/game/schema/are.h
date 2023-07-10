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

namespace schema {

struct ARE_MiniGame_Player_Gun_Banks_Bullet {
    std::string Bullet_Model;
    std::string Collision_Sound;
    uint32_t Damage {0};
    float Lifespan {0.0f};
    float Rate_Of_Fire {0.0f};
    float Speed {0.0f};
    uint32_t Target_Type {0};
};

struct ARE_MiniGame_Enemies_Gun_Banks_Bullet {
    std::string Bullet_Model;
    std::string Collision_Sound;
    uint32_t Damage {0};
    float Lifespan {0.0f};
    float Rate_Of_Fire {0.0f};
    float Speed {0.0f};
    uint32_t Target_Type {0};
};

struct ARE_MiniGame_Player_Sounds {
    std::string Death;
    std::string Engine;
};

struct ARE_MiniGame_Player_Scripts {
    std::string OnAccelerate;
    std::string OnAnimEvent;
    std::string OnBrake;
    std::string OnCreate;
    std::string OnDamage;
    std::string OnDeath;
    std::string OnFire;
    std::string OnHeartbeat;
    std::string OnHitBullet;
    std::string OnHitFollower;
    std::string OnHitObstacle;
    std::string OnHitWorld;
    std::string OnTrackLoop;
};

struct ARE_MiniGame_Player_Models {
    std::string Model;
    uint8_t RotatingModel {0};
};

struct ARE_MiniGame_Player_Gun_Banks {
    uint32_t BankID {0};
    ARE_MiniGame_Player_Gun_Banks_Bullet Bullet;
    std::string Fire_Sound;
    std::string Gun_Model;
};

struct ARE_MiniGame_Obstacles_Scripts {
    std::string OnAnimEvent;
    std::string OnCreate;
    std::string OnHeartbeat;
    std::string OnHitBullet;
    std::string OnHitFollower;
};

struct ARE_MiniGame_Enemies_Sounds {
    std::string Death;
    std::string Engine;
};

struct ARE_MiniGame_Enemies_Scripts {
    std::string OnAccelerate;
    std::string OnAnimEvent;
    std::string OnBrake;
    std::string OnCreate;
    std::string OnDamage;
    std::string OnDeath;
    std::string OnFire;
    std::string OnHeartbeat;
    std::string OnHitBullet;
    std::string OnHitFollower;
    std::string OnHitObstacle;
    std::string OnHitWorld;
    std::string OnTrackLoop;
};

struct ARE_MiniGame_Enemies_Models {
    std::string Model;
    uint8_t RotatingModel {0};
};

struct ARE_MiniGame_Enemies_Gun_Banks {
    uint32_t BankID {0};
    ARE_MiniGame_Enemies_Gun_Banks_Bullet Bullet;
    std::string Fire_Sound;
    std::string Gun_Model;
    float Horiz_Spread {0.0f};
    float Inaccuracy {0.0f};
    float Sensing_Radius {0.0f};
    float Vert_Spread {0.0f};
};

struct ARE_MiniGame_Player {
    float Accel_Secs {0.0f};
    int Bump_Damage {0};
    std::string Camera;
    uint8_t CameraRotate {0};
    std::vector<ARE_MiniGame_Player_Gun_Banks> Gun_Banks;
    uint32_t Hit_Points {0};
    float Invince_Period {0.0f};
    uint32_t Max_HPs {0};
    float Maximum_Speed {0.0f};
    float Minimum_Speed {0.0f};
    std::vector<ARE_MiniGame_Player_Models> Models;
    int Num_Loops {0};
    ARE_MiniGame_Player_Scripts Scripts;
    ARE_MiniGame_Player_Sounds Sounds;
    float Sphere_Radius {0.0f};
    float Start_Offset_X {0.0f};
    float Start_Offset_Y {0.0f};
    float Start_Offset_Z {0.0f};
    float Target_Offset_X {0.0f};
    float Target_Offset_Y {0.0f};
    float Target_Offset_Z {0.0f};
    std::string Track;
    glm::vec3 TunnelInfinite {0.0f};
    float TunnelXNeg {0.0f};
    float TunnelXPos {0.0f};
    float TunnelYNeg {0.0f};
    float TunnelYPos {0.0f};
    float TunnelZNeg {0.0f};
    float TunnelZPos {0.0f};
};

struct ARE_MiniGame_Obstacles {
    std::string Name;
    ARE_MiniGame_Obstacles_Scripts Scripts;
};

struct ARE_MiniGame_Mouse {
    uint32_t AxisX {0};
    uint32_t AxisY {0};
    uint8_t FlipAxisX {0};
    uint8_t FlipAxisY {0};
};

struct ARE_MiniGame_Enemies {
    int Bump_Damage {0};
    std::vector<ARE_MiniGame_Enemies_Gun_Banks> Gun_Banks;
    uint32_t Hit_Points {0};
    float Invince_Period {0.0f};
    uint32_t Max_HPs {0};
    std::vector<ARE_MiniGame_Enemies_Models> Models;
    int Num_Loops {0};
    ARE_MiniGame_Enemies_Scripts Scripts;
    ARE_MiniGame_Enemies_Sounds Sounds;
    float Sphere_Radius {0.0f};
    std::string Track;
    uint8_t Trigger {0};
};

struct ARE_Rooms {
    float AmbientScale {0.0f};
    uint8_t DisableWeather {0};
    int EnvAudio {0};
    int ForceRating {0};
    std::string RoomName;
};

struct ARE_MiniGame {
    uint32_t Bump_Plane {0};
    float CameraViewAngle {0.0f};
    uint32_t DOF {0};
    uint8_t DoBumping {0};
    std::vector<ARE_MiniGame_Enemies> Enemies;
    float Far_Clip {0.0f};
    float LateralAccel {0.0f};
    ARE_MiniGame_Mouse Mouse;
    float MovementPerSec {0.0f};
    std::string Music;
    float Near_Clip {0.0f};
    std::vector<ARE_MiniGame_Obstacles> Obstacles;
    ARE_MiniGame_Player Player;
    uint32_t Type {0};
    uint8_t UseInertia {0};
};

struct ARE_Map {
    float MapPt1X {0.0f};
    float MapPt1Y {0.0f};
    float MapPt2X {0.0f};
    float MapPt2Y {0.0f};
    int MapResX {0};
    int MapZoom {0};
    int NorthAxis {0};
    float WorldPt1X {0.0f};
    float WorldPt1Y {0.0f};
    float WorldPt2X {0.0f};
    float WorldPt2Y {0.0f};
};

struct ARE {
    float AlphaTest {0.0f};
    int CameraStyle {0};
    int ChanceLightning {0};
    int ChanceRain {0};
    int ChanceSnow {0};
    std::string Comments;
    int Creator_ID {0};
    uint8_t DayNightCycle {0};
    std::string DefaultEnvMap;
    int DirtyARGBOne {0};
    int DirtyARGBThree {0};
    int DirtyARGBTwo {0};
    int DirtyFormulaOne {0};
    int DirtyFormulaThre {0};
    int DirtyFormulaTwo {0};
    int DirtyFuncOne {0};
    int DirtyFuncThree {0};
    int DirtyFuncTwo {0};
    int DirtySizeOne {0};
    int DirtySizeThree {0};
    int DirtySizeTwo {0};
    uint8_t DisableTransit {0};
    uint32_t DynAmbientColor {0};
    std::vector<void *> Expansion_List;
    uint32_t Flags {0};
    uint32_t Grass_Ambient {0};
    float Grass_Density {0.0f};
    uint32_t Grass_Diffuse {0};
    uint32_t Grass_Emissive {0};
    float Grass_Prob_LL {0.0f};
    float Grass_Prob_LR {0.0f};
    float Grass_Prob_UL {0.0f};
    float Grass_Prob_UR {0.0f};
    float Grass_QuadSize {0.0f};
    std::string Grass_TexName;
    int ID {0};
    uint8_t IsNight {0};
    uint8_t LightingScheme {0};
    uint16_t LoadScreenID {0};
    ARE_Map Map;
    ARE_MiniGame MiniGame;
    int ModListenCheck {0};
    int ModSpotCheck {0};
    uint32_t MoonAmbientColor {0};
    uint32_t MoonDiffuseColor {0};
    uint32_t MoonFogColor {0};
    float MoonFogFar {0.0f};
    float MoonFogNear {0.0f};
    uint8_t MoonFogOn {0};
    uint8_t MoonShadows {0};
    std::pair<int, std::string> Name;
    uint8_t NoHangBack {0};
    uint8_t NoRest {0};
    std::string OnEnter;
    std::string OnExit;
    std::string OnHeartbeat;
    std::string OnUserDefined;
    uint8_t PlayerOnly {0};
    uint8_t PlayerVsPlayer {0};
    std::vector<ARE_Rooms> Rooms;
    uint8_t ShadowOpacity {0};
    uint8_t StealthXPEnabled {0};
    uint32_t StealthXPLoss {0};
    uint32_t StealthXPMax {0};
    uint32_t SunAmbientColor {0};
    uint32_t SunDiffuseColor {0};
    uint32_t SunFogColor {0};
    float SunFogFar {0.0f};
    float SunFogNear {0.0f};
    uint8_t SunFogOn {0};
    uint8_t SunShadows {0};
    std::string Tag;
    uint8_t Unescapable {0};
    uint32_t Version {0};
    int WindPower {0};
};

ARE parseARE(const resource::Gff &gff);

} // namespace schema

} // namespace game

} // namespace reone
