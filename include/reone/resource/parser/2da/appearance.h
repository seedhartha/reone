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

class TwoDA;

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

AppearanceTwoDARow parseAppearanceTwoDARow(const TwoDA &twoDA, int rownum);

struct AppearanceTwoDA {
    std::vector<AppearanceTwoDARow> rows;
};

AppearanceTwoDA parseAppearanceTwoDA(const TwoDA &twoDA);

} // namespace generated

} // namespace resource

} // namespace reone
