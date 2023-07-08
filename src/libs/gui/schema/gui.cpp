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

#include "reone/gui/schema/gui.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

static GUI_CONTROLS_SCROLLBAR_THUMB parseGUI_CONTROLS_SCROLLBAR_THUMB(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR_THUMB strct;
    strct.ALIGNMENT = gff.getInt("ALIGNMENT");
    strct.DRAWSTYLE = gff.getInt("DRAWSTYLE");
    strct.FLIPSTYLE = gff.getInt("FLIPSTYLE");
    strct.IMAGE = gff.getString("IMAGE");
    strct.ROTATE = gff.getFloat("ROTATE");
    return strct;
}

static GUI_CONTROLS_SCROLLBAR_EXTENT parseGUI_CONTROLS_SCROLLBAR_EXTENT(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR_EXTENT strct;
    strct.HEIGHT = gff.getInt("HEIGHT");
    strct.LEFT = gff.getInt("LEFT");
    strct.TOP = gff.getInt("TOP");
    strct.WIDTH = gff.getInt("WIDTH");
    return strct;
}

static GUI_CONTROLS_SCROLLBAR_DIR parseGUI_CONTROLS_SCROLLBAR_DIR(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR_DIR strct;
    strct.ALIGNMENT = gff.getInt("ALIGNMENT");
    strct.DRAWSTYLE = gff.getInt("DRAWSTYLE");
    strct.FLIPSTYLE = gff.getInt("FLIPSTYLE");
    strct.IMAGE = gff.getString("IMAGE");
    strct.ROTATE = gff.getFloat("ROTATE");
    return strct;
}

static GUI_CONTROLS_SCROLLBAR_BORDER parseGUI_CONTROLS_SCROLLBAR_BORDER(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR_BORDER strct;
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

static GUI_CONTROLS_PROTOITEM_TEXT parseGUI_CONTROLS_PROTOITEM_TEXT(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM_TEXT strct;
    strct.ALIGNMENT = gff.getInt("ALIGNMENT");
    strct.COLOR = gff.getVector("COLOR");
    strct.FONT = gff.getString("FONT");
    strct.PULSING = gff.getUint("PULSING");
    strct.STRREF = gff.getUint("STRREF");
    strct.TEXT = gff.getString("TEXT");
    return strct;
}

static GUI_CONTROLS_PROTOITEM_SELECTED parseGUI_CONTROLS_PROTOITEM_SELECTED(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM_SELECTED strct;
    strct.COLOR = gff.getVector("COLOR");
    strct.CORNER = gff.getString("CORNER");
    strct.DIMENSION = gff.getInt("DIMENSION");
    strct.EDGE = gff.getString("EDGE");
    strct.FILL = gff.getString("FILL");
    strct.FILLSTYLE = gff.getInt("FILLSTYLE");
    strct.INNEROFFSET = gff.getInt("INNEROFFSET");
    strct.PULSING = gff.getUint("PULSING");
    return strct;
}

static GUI_CONTROLS_PROTOITEM_HILIGHTSELECTED parseGUI_CONTROLS_PROTOITEM_HILIGHTSELECTED(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM_HILIGHTSELECTED strct;
    strct.COLOR = gff.getVector("COLOR");
    strct.CORNER = gff.getString("CORNER");
    strct.DIMENSION = gff.getInt("DIMENSION");
    strct.EDGE = gff.getString("EDGE");
    strct.FILL = gff.getString("FILL");
    strct.FILLSTYLE = gff.getInt("FILLSTYLE");
    strct.INNEROFFSET = gff.getInt("INNEROFFSET");
    strct.PULSING = gff.getUint("PULSING");
    return strct;
}

static GUI_CONTROLS_PROTOITEM_HILIGHT parseGUI_CONTROLS_PROTOITEM_HILIGHT(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM_HILIGHT strct;
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

static GUI_CONTROLS_PROTOITEM_EXTENT parseGUI_CONTROLS_PROTOITEM_EXTENT(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM_EXTENT strct;
    strct.HEIGHT = gff.getInt("HEIGHT");
    strct.LEFT = gff.getInt("LEFT");
    strct.TOP = gff.getInt("TOP");
    strct.WIDTH = gff.getInt("WIDTH");
    return strct;
}

static GUI_CONTROLS_PROTOITEM_BORDER parseGUI_CONTROLS_PROTOITEM_BORDER(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM_BORDER strct;
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

static GUI_CONTROLS_TEXT parseGUI_CONTROLS_TEXT(const Gff &gff) {
    GUI_CONTROLS_TEXT strct;
    strct.ALIGNMENT = gff.getInt("ALIGNMENT");
    strct.COLOR = gff.getVector("COLOR");
    strct.FONT = gff.getString("FONT");
    strct.PULSING = gff.getUint("PULSING");
    strct.STRREF = gff.getUint("STRREF");
    strct.TEXT = gff.getString("TEXT");
    return strct;
}

static GUI_CONTROLS_SCROLLBAR parseGUI_CONTROLS_SCROLLBAR(const Gff &gff) {
    GUI_CONTROLS_SCROLLBAR strct;
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_CONTROLS_SCROLLBAR_BORDER(*BORDER);
    }
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    strct.CURVALUE = gff.getInt("CURVALUE");
    auto DIR = gff.findStruct("DIR");
    if (DIR) {
        strct.DIR = parseGUI_CONTROLS_SCROLLBAR_DIR(*DIR);
    }
    strct.DRAWMODE = gff.getUint("DRAWMODE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_CONTROLS_SCROLLBAR_EXTENT(*EXTENT);
    }
    strct.MAXVALUE = gff.getInt("MAXVALUE");
    strct.Obj_Parent = gff.getString("Obj_Parent");
    strct.Obj_ParentID = gff.getInt("Obj_ParentID");
    strct.TAG = gff.getString("TAG");
    auto THUMB = gff.findStruct("THUMB");
    if (THUMB) {
        strct.THUMB = parseGUI_CONTROLS_SCROLLBAR_THUMB(*THUMB);
    }
    strct.VISIBLEVALUE = gff.getInt("VISIBLEVALUE");
    return strct;
}

static GUI_CONTROLS_PROTOITEM parseGUI_CONTROLS_PROTOITEM(const Gff &gff) {
    GUI_CONTROLS_PROTOITEM strct;
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_CONTROLS_PROTOITEM_BORDER(*BORDER);
    }
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_CONTROLS_PROTOITEM_EXTENT(*EXTENT);
    }
    auto HILIGHT = gff.findStruct("HILIGHT");
    if (HILIGHT) {
        strct.HILIGHT = parseGUI_CONTROLS_PROTOITEM_HILIGHT(*HILIGHT);
    }
    auto HILIGHTSELECTED = gff.findStruct("HILIGHTSELECTED");
    if (HILIGHTSELECTED) {
        strct.HILIGHTSELECTED = parseGUI_CONTROLS_PROTOITEM_HILIGHTSELECTED(*HILIGHTSELECTED);
    }
    strct.ISSELECTED = gff.getUint("ISSELECTED");
    strct.Obj_Parent = gff.getString("Obj_Parent");
    strct.Obj_ParentID = gff.getInt("Obj_ParentID");
    auto SELECTED = gff.findStruct("SELECTED");
    if (SELECTED) {
        strct.SELECTED = parseGUI_CONTROLS_PROTOITEM_SELECTED(*SELECTED);
    }
    strct.TAG = gff.getString("TAG");
    auto TEXT = gff.findStruct("TEXT");
    if (TEXT) {
        strct.TEXT = parseGUI_CONTROLS_PROTOITEM_TEXT(*TEXT);
    }
    return strct;
}

static GUI_CONTROLS_PROGRESS parseGUI_CONTROLS_PROGRESS(const Gff &gff) {
    GUI_CONTROLS_PROGRESS strct;
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

static GUI_CONTROLS_MOVETO parseGUI_CONTROLS_MOVETO(const Gff &gff) {
    GUI_CONTROLS_MOVETO strct;
    strct.DOWN = gff.getInt("DOWN");
    strct.LEFT = gff.getInt("LEFT");
    strct.RIGHT = gff.getInt("RIGHT");
    strct.UP = gff.getInt("UP");
    return strct;
}

static GUI_CONTROLS_HILIGHT parseGUI_CONTROLS_HILIGHT(const Gff &gff) {
    GUI_CONTROLS_HILIGHT strct;
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

static GUI_CONTROLS_EXTENT parseGUI_CONTROLS_EXTENT(const Gff &gff) {
    GUI_CONTROLS_EXTENT strct;
    strct.HEIGHT = gff.getInt("HEIGHT");
    strct.LEFT = gff.getInt("LEFT");
    strct.TOP = gff.getInt("TOP");
    strct.WIDTH = gff.getInt("WIDTH");
    return strct;
}

static GUI_CONTROLS_BORDER parseGUI_CONTROLS_BORDER(const Gff &gff) {
    GUI_CONTROLS_BORDER strct;
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

static GUI_EXTENT parseGUI_EXTENT(const Gff &gff) {
    GUI_EXTENT strct;
    strct.HEIGHT = gff.getInt("HEIGHT");
    strct.LEFT = gff.getInt("LEFT");
    strct.TOP = gff.getInt("TOP");
    strct.WIDTH = gff.getInt("WIDTH");
    return strct;
}

static GUI_CONTROLS parseGUI_CONTROLS(const Gff &gff) {
    GUI_CONTROLS strct;
    auto BORDER = gff.findStruct("BORDER");
    if (BORDER) {
        strct.BORDER = parseGUI_CONTROLS_BORDER(*BORDER);
    }
    strct.COLOR = gff.getVector("COLOR");
    strct.CONTROLTYPE = gff.getInt("CONTROLTYPE");
    strct.CURVALUE = gff.getInt("CURVALUE");
    auto EXTENT = gff.findStruct("EXTENT");
    if (EXTENT) {
        strct.EXTENT = parseGUI_CONTROLS_EXTENT(*EXTENT);
    }
    auto HILIGHT = gff.findStruct("HILIGHT");
    if (HILIGHT) {
        strct.HILIGHT = parseGUI_CONTROLS_HILIGHT(*HILIGHT);
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
        strct.PROGRESS = parseGUI_CONTROLS_PROGRESS(*PROGRESS);
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
        strct.TEXT = parseGUI_CONTROLS_TEXT(*TEXT);
    }
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

} // namespace game

} // namespace reone
