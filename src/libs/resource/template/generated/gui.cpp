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

#include "reone/resource/template/generated/gui.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

namespace generated {

static GUI_EXTENT parseGUI_EXTENT(const Gff &gff) {
    GUI_EXTENT strct;
    strct.HEIGHT = gff.getInt("HEIGHT");
    strct.LEFT = gff.getInt("LEFT");
    strct.TOP = gff.getInt("TOP");
    strct.WIDTH = gff.getInt("WIDTH");
    return strct;
}

static GUI_BORDER parseGUI_BORDER(const Gff &gff) {
    GUI_BORDER strct;
    strct.COLOR = gff.getVector("COLOR");
    strct.CORNER = gff.getString("CORNER");
    strct.DIMENSION = gff.getInt("DIMENSION");
    strct.EDGE = gff.getString("EDGE");
    strct.FILL = gff.getString("FILL");
    strct.FILLSTYLE = gff.getInt("FILLSTYLE");
    strct.INNEROFFSET = gff.getInt("INNEROFFSET");
    strct.INNEROFFSETY = gff.getInt("INNEROFFSETY");
    strct.PULSING = gff.getUint("PULSING");
    return strct;
}

static GUI_TEXT parseGUI_TEXT(const Gff &gff) {
    GUI_TEXT strct;
    strct.ALIGNMENT = gff.getInt("ALIGNMENT");
    strct.COLOR = gff.getVector("COLOR");
    strct.FONT = gff.getString("FONT");
    strct.PULSING = gff.getUint("PULSING");
    strct.STRREF = gff.getUint("STRREF");
    strct.TEXT = gff.getString("TEXT");
    return strct;
}

static GUI_CONTROLS_SCROLLBAR_DIRTHUMB parseGUI_CONTROLS_SCROLLBAR_DIRTHUMB(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR_DIRTHUMB strct;
    strct.ALIGNMENT = gff.getInt("ALIGNMENT");
    strct.DRAWSTYLE = gff.getInt("DRAWSTYLE");
    strct.FLIPSTYLE = gff.getInt("FLIPSTYLE");
    strct.IMAGE = gff.getString("IMAGE");
    strct.ROTATE = gff.getFloat("ROTATE");
    return strct;
}

static GUI_CONTROLS_SCROLLBAR parseGUI_CONTROLS_SCROLLBAR(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR strct;
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_BORDER(*BORDER);
    }
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    strct.CURVALUE = gff.getInt("CURVALUE");
    auto DIR = gff.findStruct("DIR");
    if (DIR) {
        strct.DIR = parseGUI_CONTROLS_SCROLLBAR_DIRTHUMB(*DIR);
    }
    strct.DRAWMODE = gff.getUint("DRAWMODE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_EXTENT(*EXTENT);
    }
    strct.MAXVALUE = gff.getInt("MAXVALUE");
    strct.Obj_Parent = gff.getString("Obj_Parent");
    strct.Obj_ParentID = gff.getInt("Obj_ParentID");
    strct.TAG = gff.getString("TAG");
    auto THUMB = gff.findStruct("THUMB");
    if (THUMB) {
        strct.THUMB = parseGUI_CONTROLS_SCROLLBAR_DIRTHUMB(*THUMB);
    }
    strct.VISIBLEVALUE = gff.getInt("VISIBLEVALUE");
    return strct;
}

static GUI_CONTROLS_PROTOITEM parseGUI_CONTROLS_PROTOITEM(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM strct;
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_BORDER(*BORDER);
    }
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_EXTENT(*EXTENT);
    }
    auto HILIGHT = gff.findStruct("HILIGHT");
    if (HILIGHT) {
        strct.HILIGHT = parseGUI_BORDER(*HILIGHT);
    }
    auto HILIGHTSELECTED = gff.findStruct("HILIGHTSELECTED");
    if (HILIGHTSELECTED) {
        strct.HILIGHTSELECTED = parseGUI_BORDER(*HILIGHTSELECTED);
    }
    strct.ISSELECTED = gff.getUint("ISSELECTED");
    strct.Obj_Parent = gff.getString("Obj_Parent");
    strct.Obj_ParentID = gff.getInt("Obj_ParentID");
    auto SELECTED = gff.findStruct("SELECTED");
    if (SELECTED) {
        strct.SELECTED = parseGUI_BORDER(*SELECTED);
    }
    strct.TAG = gff.getString("TAG");
    auto TEXT = gff.findStruct("TEXT");
    if (TEXT) {
        strct.TEXT = parseGUI_TEXT(*TEXT);
    }
    return strct;
}

static GUI_CONTROLS_MOVETO parseGUI_CONTROLS_MOVETO(const Gff &gff) {
    GUI_CONTROLS_MOVETO strct;
    strct.DOWN = gff.getInt("DOWN");
    strct.LEFT = gff.getInt("LEFT");
    strct.RIGHT = gff.getInt("RIGHT");
    strct.UP = gff.getInt("UP");
    return strct;
}

static GUI_CONTROLS parseGUI_CONTROLS(const Gff &gff) {
    GUI_CONTROLS strct;
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_BORDER(*BORDER);
    }
    strct.COLOR = gff.getVector("COLOR");
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    strct.CURVALUE = gff.getInt("CURVALUE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_EXTENT(*EXTENT);
    }
    auto HILIGHT = gff.findStruct("HILIGHT");
    if (HILIGHT) {
        strct.HILIGHT = parseGUI_BORDER(*HILIGHT);
    }
    strct.ID = gff.getInt("ID");
    strct.LEFTSCROLLBAR = gff.getUint("LEFTSCROLLBAR");
    strct.LOOPING = gff.getUint("LOOPING");
    strct.MAXVALUE = gff.getInt("MAXVALUE");
    auto MOVETO = gff.findStruct("MOVETO");
    if (MOVETO) {
        strct.MOVETO = parseGUI_CONTROLS_MOVETO(*MOVETO);
    }
    strct.Obj_Locked = gff.getUint("Obj_Locked");
    strct.Obj_Parent = gff.getString("Obj_Parent");
    strct.Obj_ParentID = gff.getInt("Obj_ParentID");
    strct.PADDING = gff.getInt("PADDING");
    auto PROGRESS = gff.findStruct("PROGRESS");
    if (PROGRESS) {
        strct.PROGRESS = parseGUI_BORDER(*PROGRESS);
    }
    auto PROTOITEM = gff.findStruct("PROTOITEM");
    if (PROTOITEM) {
        strct.PROTOITEM = parseGUI_CONTROLS_PROTOITEM(*PROTOITEM);
    }
    auto SCROLLBAR = gff.findStruct("SCROLLBAR");
    if (SCROLLBAR) {
        strct.SCROLLBAR = parseGUI_CONTROLS_SCROLLBAR(*SCROLLBAR);
    }
    strct.STARTFROMLEFT = gff.getUint("STARTFROMLEFT");
    strct.TAG = gff.getString("TAG");
    auto TEXT = gff.findStruct("TEXT");
    if (TEXT) {
        strct.TEXT = parseGUI_TEXT(*TEXT);
    }
    return strct;
}

GUI parseGUI(const Gff &gff) {
    GUI strct;
    strct.ALPHA = gff.getFloat("ALPHA");
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_BORDER(*BORDER);
    }
    strct.COLOR = gff.getVector("COLOR");
    for (auto &item : gff.getList("CONTROLS")) {
        strct.CONTROLS.push_back(parseGUI_CONTROLS(*item));
    }
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_EXTENT(*EXTENT);
    }
    strct.Obj_Locked = gff.getUint("Obj_Locked");
    strct.Obj_ParentID = gff.getInt("Obj_ParentID");
    strct.TAG = gff.getString("TAG");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
