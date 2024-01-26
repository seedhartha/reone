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

struct AppearanceTwoDARow {
    bool abortonparry;
    std::optional<std::string> armorSound;
    std::optional<int> backuphead;
    std::string bloodcolr;
    std::optional<int> bodyBag;
    std::optional<std::string> cameraheightoffset;
    std::optional<float> cameraspace;
    std::optional<std::string> combatSound;
    float creperspace;
    std::optional<int> deathvfx;
    std::optional<std::string> deathvfxnode;
    std::optional<bool> destroyobjectdelay;
    std::optional<bool> disableinjuredanim;
    int driveaccl;
    float driveanimrunPc;
    float driveanimrunXbox;
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
    int headArcH;
    int headArcV;
    std::optional<std::string> headbone;
    std::optional<std::string> headtexe;
    std::optional<std::string> headtexg;
    std::optional<std::string> headtexve;
    std::optional<std::string> headtexvg;
    bool headtrack;
    float height;
    std::optional<std::string> helmetScaleF;
    std::optional<std::string> helmetScaleM;
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
    std::optional<int> stringRef;
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
    bool wingTailScale;
};

struct AppearanceTwoDA {
    std::vector<AppearanceTwoDARow> rows;
};

AppearanceTwoDARow parseAppearanceTwoDARow(const TwoDA &twoDA, int rownum) {
    AppearanceTwoDARow row;
    row.abortonparry = twoDA.getBool(rownum, "abortonparry");
    row.armorSound = twoDA.getStringOpt(rownum, "armorSound");
    row.backuphead = twoDA.getIntOpt(rownum, "backuphead");
    row.bloodcolr = twoDA.getString(rownum, "bloodcolr");
    row.bodyBag = twoDA.getIntOpt(rownum, "bodyBag");
    row.cameraheightoffset = twoDA.getStringOpt(rownum, "cameraheightoffset");
    row.cameraspace = twoDA.getFloatOpt(rownum, "cameraspace");
    row.combatSound = twoDA.getStringOpt(rownum, "combatSound");
    row.creperspace = twoDA.getFloat(rownum, "creperspace");
    row.deathvfx = twoDA.getIntOpt(rownum, "deathvfx");
    row.deathvfxnode = twoDA.getStringOpt(rownum, "deathvfxnode");
    row.destroyobjectdelay = twoDA.getBoolOpt(rownum, "destroyobjectdelay");
    row.disableinjuredanim = twoDA.getBoolOpt(rownum, "disableinjuredanim");
    row.driveaccl = twoDA.getInt(rownum, "driveaccl");
    row.driveanimrunPc = twoDA.getFloat(rownum, "driveanimrunPc");
    row.driveanimrunXbox = twoDA.getFloat(rownum, "driveanimrunXbox");
    row.driveanimwalk = twoDA.getFloat(rownum, "driveanimwalk");
    row.drivemaxspeed = twoDA.getFloat(rownum, "drivemaxspeed");
    row.envmap = twoDA.getStringOpt(rownum, "envmap");
    row.equipslotslocked = twoDA.getIntOpt(rownum, "equipslotslocked");
    row.fadedelayondeath = twoDA.getBoolOpt(rownum, "fadedelayondeath");
    row.footstepsound = twoDA.getStringOpt(rownum, "footstepsound");
    row.footsteptype = twoDA.getIntOpt(rownum, "footsteptype");
    row.footstepvolume = twoDA.getBool(rownum, "footstepvolume");
    row.freelookeffect = twoDA.getIntOpt(rownum, "freelookeffect");
    row.groundtilt = twoDA.getBool(rownum, "groundtilt");
    row.hasarms = twoDA.getBool(rownum, "hasarms");
    row.haslegs = twoDA.getBool(rownum, "haslegs");
    row.headArcH = twoDA.getInt(rownum, "headArcH");
    row.headArcV = twoDA.getInt(rownum, "headArcV");
    row.headbone = twoDA.getStringOpt(rownum, "headbone");
    row.headtexe = twoDA.getStringOpt(rownum, "headtexe");
    row.headtexg = twoDA.getStringOpt(rownum, "headtexg");
    row.headtexve = twoDA.getStringOpt(rownum, "headtexve");
    row.headtexvg = twoDA.getStringOpt(rownum, "headtexvg");
    row.headtrack = twoDA.getBool(rownum, "headtrack");
    row.height = twoDA.getFloat(rownum, "height");
    row.helmetScaleF = twoDA.getStringOpt(rownum, "helmetScaleF");
    row.helmetScaleM = twoDA.getStringOpt(rownum, "helmetScaleM");
    row.hitdist = twoDA.getFloat(rownum, "hitdist");
    row.hitradius = twoDA.getFloat(rownum, "hitradius");
    row.label = twoDA.getStringOpt(rownum, "label");
    row.modela = twoDA.getStringOpt(rownum, "modela");
    row.modelb = twoDA.getStringOpt(rownum, "modelb");
    row.modelc = twoDA.getStringOpt(rownum, "modelc");
    row.modeld = twoDA.getStringOpt(rownum, "modeld");
    row.modele = twoDA.getStringOpt(rownum, "modele");
    row.modelf = twoDA.getStringOpt(rownum, "modelf");
    row.modelg = twoDA.getStringOpt(rownum, "modelg");
    row.modelh = twoDA.getStringOpt(rownum, "modelh");
    row.modeli = twoDA.getStringOpt(rownum, "modeli");
    row.modelj = twoDA.getStringOpt(rownum, "modelj");
    row.modelk = twoDA.getStringOpt(rownum, "modelk");
    row.modell = twoDA.getStringOpt(rownum, "modell");
    row.modelm = twoDA.getStringOpt(rownum, "modelm");
    row.modeln = twoDA.getStringOpt(rownum, "modeln");
    row.modeltype = twoDA.getString(rownum, "modeltype");
    row.moverate = twoDA.getString(rownum, "moverate");
    row.normalhead = twoDA.getIntOpt(rownum, "normalhead");
    row.perceptiondist = twoDA.getInt(rownum, "perceptiondist");
    row.perspace = twoDA.getFloat(rownum, "perspace");
    row.portrait = twoDA.getStringOpt(rownum, "portrait");
    row.prefatckdist = twoDA.getFloat(rownum, "prefatckdist");
    row.race = twoDA.getStringOpt(rownum, "race");
    row.racetex = twoDA.getStringOpt(rownum, "racetex");
    row.racialtype = twoDA.getInt(rownum, "racialtype");
    row.rundist = twoDA.getFloat(rownum, "rundist");
    row.sizecategory = twoDA.getInt(rownum, "sizecategory");
    row.skin = twoDA.getStringOpt(rownum, "skin");
    row.soundapptype = twoDA.getIntOpt(rownum, "soundapptype");
    row.stringRef = twoDA.getIntOpt(rownum, "stringRef");
    row.targetheight = twoDA.getString(rownum, "targetheight");
    row.texa = twoDA.getStringOpt(rownum, "texa");
    row.texaevil = twoDA.getStringOpt(rownum, "texaevil");
    row.texb = twoDA.getStringOpt(rownum, "texb");
    row.texbevil = twoDA.getStringOpt(rownum, "texbevil");
    row.texc = twoDA.getStringOpt(rownum, "texc");
    row.texd = twoDA.getStringOpt(rownum, "texd");
    row.texe = twoDA.getStringOpt(rownum, "texe");
    row.texf = twoDA.getStringOpt(rownum, "texf");
    row.texg = twoDA.getStringOpt(rownum, "texg");
    row.texh = twoDA.getStringOpt(rownum, "texh");
    row.texi = twoDA.getStringOpt(rownum, "texi");
    row.texievil = twoDA.getStringOpt(rownum, "texievil");
    row.texj = twoDA.getStringOpt(rownum, "texj");
    row.texk = twoDA.getStringOpt(rownum, "texk");
    row.texl = twoDA.getStringOpt(rownum, "texl");
    row.texlevil = twoDA.getStringOpt(rownum, "texlevil");
    row.texm = twoDA.getStringOpt(rownum, "texm");
    row.texn = twoDA.getStringOpt(rownum, "texn");
    row.texnevil = twoDA.getStringOpt(rownum, "texnevil");
    row.walkdist = twoDA.getFloat(rownum, "walkdist");
    row.weaponscale = twoDA.getStringOpt(rownum, "weaponscale");
    row.wingTailScale = twoDA.getBool(rownum, "wingTailScale");
    return row;
}

AppearanceTwoDA parseAppearanceTwoDA(const TwoDA &twoDA) {
    AppearanceTwoDA strct;
    for (int i = 0; i < twoDA.getRowCount(); ++i) {
        strct.rows.push_back(parseAppearanceTwoDARow(twoDA, i));
    }
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone

