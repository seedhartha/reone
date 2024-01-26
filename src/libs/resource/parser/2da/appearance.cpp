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

#include "reone/resource/parser/2da/appearance.h"

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

namespace generated {

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
