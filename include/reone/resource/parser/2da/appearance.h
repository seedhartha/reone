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

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

namespace generated {

struct appearance {
    bool abortonparry;
    std::optional<std::string> armor_sound;
    std::optional<int> backuphead;
    std::string bloodcolr;
    std::optional<int> body_bag;
    std::optional<std::string> cameraheightoffset;
    std::optional<float> cameraspace;
    std::optional<std::string> combat_sound;
    float creperspace;
    std::optional<int> deathvfx;
    std::optional<std::string> deathvfxnode;
    std::optional<bool> destroyobjectdelay;
    std::optional<bool> disableinjuredanim;
    int driveaccl;
    float driveanimrun_pc;
    float driveanimrun_xbox;
    float driveanimwalk;
    float drivemaxspeed;
    std::optional<std::string> envmap;
    std::optional<int> equipslotslocked;
    std::optional<bool> fadedelayondeath;
    std::optional<std::string> footstepsound;
    std::optional<int> footsteptype;
    bool footstepvolume;
    std::optional<int> freelookeffect;
    bool groundtilt;
    bool hasarms;
    bool haslegs;
    int head_arc_h;
    int head_arc_v;
    std::optional<std::string> headbone;
    std::optional<std::string> headtexe;
    std::optional<std::string> headtexg;
    std::optional<std::string> headtexve;
    std::optional<std::string> headtexvg;
    bool headtrack;
    float height;
    std::optional<std::string> helmet_scale_f;
    std::optional<std::string> helmet_scale_m;
    float hitdist;
    float hitradius;
    std::optional<std::string> label;
    std::optional<std::string> modela;
    std::optional<std::string> modelb;
    std::optional<std::string> modelc;
    std::optional<std::string> modeld;
    std::optional<std::string> modele;
    std::optional<std::string> modelf;
    std::optional<std::string> modelg;
    std::optional<std::string> modelh;
    std::optional<std::string> modeli;
    std::optional<std::string> modelj;
    std::optional<std::string> modelk;
    std::optional<std::string> modell;
    std::optional<std::string> modelm;
    std::optional<std::string> modeln;
    std::string modeltype;
    std::string moverate;
    std::optional<int> normalhead;
    int perceptiondist;
    float perspace;
    std::optional<std::string> portrait;
    float prefatckdist;
    std::optional<std::string> race;
    std::optional<std::string> racetex;
    int racialtype;
    float rundist;
    int sizecategory;
    std::optional<std::string> skin;
    std::optional<int> soundapptype;
    std::optional<int> string_ref;
    std::string targetheight;
    std::optional<std::string> texa;
    std::optional<std::string> texaevil;
    std::optional<std::string> texb;
    std::optional<std::string> texbevil;
    std::optional<std::string> texc;
    std::optional<std::string> texd;
    std::optional<std::string> texe;
    std::optional<std::string> texf;
    std::optional<std::string> texg;
    std::optional<std::string> texh;
    std::optional<std::string> texi;
    std::optional<std::string> texievil;
    std::optional<std::string> texj;
    std::optional<std::string> texk;
    std::optional<std::string> texl;
    std::optional<std::string> texlevil;
    std::optional<std::string> texm;
    std::optional<std::string> texn;
    std::optional<std::string> texnevil;
    float walkdist;
    std::optional<std::string> weaponscale;
    bool wing_tail_scale;
};

appearance parse_appearance(const TwoDA &twoDA, int row) {
    appearance strct;
    strct.abortonparry = twoDA.getBool(row, "abortonparry");
    strct.armor_sound = twoDA.getStringOpt(row, "armor_sound");
    strct.backuphead = twoDA.getIntOpt(row, "backuphead");
    strct.bloodcolr = twoDA.getString(row, "bloodcolr");
    strct.body_bag = twoDA.getIntOpt(row, "body_bag");
    strct.cameraheightoffset = twoDA.getStringOpt(row, "cameraheightoffset");
    strct.cameraspace = twoDA.getFloatOpt(row, "cameraspace");
    strct.combat_sound = twoDA.getStringOpt(row, "combat_sound");
    strct.creperspace = twoDA.getFloat(row, "creperspace");
    strct.deathvfx = twoDA.getIntOpt(row, "deathvfx");
    strct.deathvfxnode = twoDA.getStringOpt(row, "deathvfxnode");
    strct.destroyobjectdelay = twoDA.getBoolOpt(row, "destroyobjectdelay");
    strct.disableinjuredanim = twoDA.getBoolOpt(row, "disableinjuredanim");
    strct.driveaccl = twoDA.getInt(row, "driveaccl");
    strct.driveanimrun_pc = twoDA.getFloat(row, "driveanimrun_pc");
    strct.driveanimrun_xbox = twoDA.getFloat(row, "driveanimrun_xbox");
    strct.driveanimwalk = twoDA.getFloat(row, "driveanimwalk");
    strct.drivemaxspeed = twoDA.getFloat(row, "drivemaxspeed");
    strct.envmap = twoDA.getStringOpt(row, "envmap");
    strct.equipslotslocked = twoDA.getIntOpt(row, "equipslotslocked");
    strct.fadedelayondeath = twoDA.getBoolOpt(row, "fadedelayondeath");
    strct.footstepsound = twoDA.getStringOpt(row, "footstepsound");
    strct.footsteptype = twoDA.getIntOpt(row, "footsteptype");
    strct.footstepvolume = twoDA.getBool(row, "footstepvolume");
    strct.freelookeffect = twoDA.getIntOpt(row, "freelookeffect");
    strct.groundtilt = twoDA.getBool(row, "groundtilt");
    strct.hasarms = twoDA.getBool(row, "hasarms");
    strct.haslegs = twoDA.getBool(row, "haslegs");
    strct.head_arc_h = twoDA.getInt(row, "head_arc_h");
    strct.head_arc_v = twoDA.getInt(row, "head_arc_v");
    strct.headbone = twoDA.getStringOpt(row, "headbone");
    strct.headtexe = twoDA.getStringOpt(row, "headtexe");
    strct.headtexg = twoDA.getStringOpt(row, "headtexg");
    strct.headtexve = twoDA.getStringOpt(row, "headtexve");
    strct.headtexvg = twoDA.getStringOpt(row, "headtexvg");
    strct.headtrack = twoDA.getBool(row, "headtrack");
    strct.height = twoDA.getFloat(row, "height");
    strct.helmet_scale_f = twoDA.getStringOpt(row, "helmet_scale_f");
    strct.helmet_scale_m = twoDA.getStringOpt(row, "helmet_scale_m");
    strct.hitdist = twoDA.getFloat(row, "hitdist");
    strct.hitradius = twoDA.getFloat(row, "hitradius");
    strct.label = twoDA.getStringOpt(row, "label");
    strct.modela = twoDA.getStringOpt(row, "modela");
    strct.modelb = twoDA.getStringOpt(row, "modelb");
    strct.modelc = twoDA.getStringOpt(row, "modelc");
    strct.modeld = twoDA.getStringOpt(row, "modeld");
    strct.modele = twoDA.getStringOpt(row, "modele");
    strct.modelf = twoDA.getStringOpt(row, "modelf");
    strct.modelg = twoDA.getStringOpt(row, "modelg");
    strct.modelh = twoDA.getStringOpt(row, "modelh");
    strct.modeli = twoDA.getStringOpt(row, "modeli");
    strct.modelj = twoDA.getStringOpt(row, "modelj");
    strct.modelk = twoDA.getStringOpt(row, "modelk");
    strct.modell = twoDA.getStringOpt(row, "modell");
    strct.modelm = twoDA.getStringOpt(row, "modelm");
    strct.modeln = twoDA.getStringOpt(row, "modeln");
    strct.modeltype = twoDA.getString(row, "modeltype");
    strct.moverate = twoDA.getString(row, "moverate");
    strct.normalhead = twoDA.getIntOpt(row, "normalhead");
    strct.perceptiondist = twoDA.getInt(row, "perceptiondist");
    strct.perspace = twoDA.getFloat(row, "perspace");
    strct.portrait = twoDA.getStringOpt(row, "portrait");
    strct.prefatckdist = twoDA.getFloat(row, "prefatckdist");
    strct.race = twoDA.getStringOpt(row, "race");
    strct.racetex = twoDA.getStringOpt(row, "racetex");
    strct.racialtype = twoDA.getInt(row, "racialtype");
    strct.rundist = twoDA.getFloat(row, "rundist");
    strct.sizecategory = twoDA.getInt(row, "sizecategory");
    strct.skin = twoDA.getStringOpt(row, "skin");
    strct.soundapptype = twoDA.getIntOpt(row, "soundapptype");
    strct.string_ref = twoDA.getIntOpt(row, "string_ref");
    strct.targetheight = twoDA.getString(row, "targetheight");
    strct.texa = twoDA.getStringOpt(row, "texa");
    strct.texaevil = twoDA.getStringOpt(row, "texaevil");
    strct.texb = twoDA.getStringOpt(row, "texb");
    strct.texbevil = twoDA.getStringOpt(row, "texbevil");
    strct.texc = twoDA.getStringOpt(row, "texc");
    strct.texd = twoDA.getStringOpt(row, "texd");
    strct.texe = twoDA.getStringOpt(row, "texe");
    strct.texf = twoDA.getStringOpt(row, "texf");
    strct.texg = twoDA.getStringOpt(row, "texg");
    strct.texh = twoDA.getStringOpt(row, "texh");
    strct.texi = twoDA.getStringOpt(row, "texi");
    strct.texievil = twoDA.getStringOpt(row, "texievil");
    strct.texj = twoDA.getStringOpt(row, "texj");
    strct.texk = twoDA.getStringOpt(row, "texk");
    strct.texl = twoDA.getStringOpt(row, "texl");
    strct.texlevil = twoDA.getStringOpt(row, "texlevil");
    strct.texm = twoDA.getStringOpt(row, "texm");
    strct.texn = twoDA.getStringOpt(row, "texn");
    strct.texnevil = twoDA.getStringOpt(row, "texnevil");
    strct.walkdist = twoDA.getFloat(row, "walkdist");
    strct.weaponscale = twoDA.getStringOpt(row, "weaponscale");
    strct.wing_tail_scale = twoDA.getBool(row, "wing_tail_scale");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

