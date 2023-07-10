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

namespace gui {

namespace schema {

struct GUI_CONTROLS_SCROLLBAR_THUMB {
    int ALIGNMENT {0};
    int DRAWSTYLE {0};
    int FLIPSTYLE {0};
    std::string IMAGE;
    float ROTATE {0.0f};
};

struct GUI_CONTROLS_SCROLLBAR_EXTENT {
    int HEIGHT {0};
    int LEFT {0};
    int TOP {0};
    int WIDTH {0};
};

struct GUI_CONTROLS_SCROLLBAR_DIR {
    int ALIGNMENT {0};
    int DRAWSTYLE {0};
    int FLIPSTYLE {0};
    std::string IMAGE;
    float ROTATE {0.0f};
};

struct GUI_CONTROLS_SCROLLBAR_BORDER {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_PROTOITEM_TEXT {
    int ALIGNMENT {0};
    glm::vec3 COLOR {0.0f};
    std::string FONT;
    uint8_t PULSING {0};
    uint32_t STRREF {0};
    std::string TEXT;
};

struct GUI_CONTROLS_PROTOITEM_SELECTED {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_PROTOITEM_HILIGHTSELECTED {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_PROTOITEM_HILIGHT {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_PROTOITEM_EXTENT {
    int HEIGHT {0};
    int LEFT {0};
    int TOP {0};
    int WIDTH {0};
};

struct GUI_CONTROLS_PROTOITEM_BORDER {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_TEXT {
    int ALIGNMENT {0};
    glm::vec3 COLOR {0.0f};
    std::string FONT;
    uint8_t PULSING {0};
    uint32_t STRREF {0};
    std::string TEXT;
};

struct GUI_CONTROLS_SCROLLBAR {
    GUI_CONTROLS_SCROLLBAR_BORDER BORDER;
    int CONTROLTYPE {0};
    int CURVALUE {0};
    GUI_CONTROLS_SCROLLBAR_DIR DIR;
    uint8_t DRAWMODE {0};
    GUI_CONTROLS_SCROLLBAR_EXTENT EXTENT;
    int MAXVALUE {0};
    std::string Obj_Parent;
    int Obj_ParentID {0};
    std::string TAG;
    GUI_CONTROLS_SCROLLBAR_THUMB THUMB;
    int VISIBLEVALUE {0};
};

struct GUI_CONTROLS_PROTOITEM {
    GUI_CONTROLS_PROTOITEM_BORDER BORDER;
    int CONTROLTYPE {0};
    GUI_CONTROLS_PROTOITEM_EXTENT EXTENT;
    GUI_CONTROLS_PROTOITEM_HILIGHT HILIGHT;
    GUI_CONTROLS_PROTOITEM_HILIGHTSELECTED HILIGHTSELECTED;
    uint8_t ISSELECTED {0};
    std::string Obj_Parent;
    int Obj_ParentID {0};
    GUI_CONTROLS_PROTOITEM_SELECTED SELECTED;
    std::string TAG;
    GUI_CONTROLS_PROTOITEM_TEXT TEXT;
};

struct GUI_CONTROLS_PROGRESS {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_MOVETO {
    int DOWN {0};
    int LEFT {0};
    int RIGHT {0};
    int UP {0};
};

struct GUI_CONTROLS_HILIGHT {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI_CONTROLS_EXTENT {
    int HEIGHT {0};
    int LEFT {0};
    int TOP {0};
    int WIDTH {0};
};

struct GUI_CONTROLS_BORDER {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI_EXTENT {
    int HEIGHT {0};
    int LEFT {0};
    int TOP {0};
    int WIDTH {0};
};

struct GUI_CONTROLS {
    GUI_CONTROLS_BORDER BORDER;
    glm::vec3 COLOR {0.0f};
    int CONTROLTYPE {0};
    int CURVALUE {0};
    GUI_CONTROLS_EXTENT EXTENT;
    GUI_CONTROLS_HILIGHT HILIGHT;
    int ID {0};
    uint8_t LEFTSCROLLBAR {0};
    uint8_t LOOPING {0};
    int MAXVALUE {0};
    GUI_CONTROLS_MOVETO MOVETO;
    uint8_t Obj_Locked {0};
    std::string Obj_Parent;
    int Obj_ParentID {0};
    int PADDING {0};
    GUI_CONTROLS_PROGRESS PROGRESS;
    GUI_CONTROLS_PROTOITEM PROTOITEM;
    GUI_CONTROLS_SCROLLBAR SCROLLBAR;
    uint8_t STARTFROMLEFT {0};
    std::string TAG;
    GUI_CONTROLS_TEXT TEXT;
};

struct GUI_BORDER {
    glm::vec3 COLOR {0.0f};
    std::string CORNER;
    int DIMENSION {0};
    std::string EDGE;
    std::string FILL;
    int FILLSTYLE {0};
    int INNEROFFSET {0};
    int INNEROFFSETY {0};
    uint8_t PULSING {0};
};

struct GUI {
    float ALPHA {0.0f};
    GUI_BORDER BORDER;
    glm::vec3 COLOR {0.0f};
    std::vector<GUI_CONTROLS> CONTROLS;
    int CONTROLTYPE {0};
    GUI_EXTENT EXTENT;
    uint8_t Obj_Locked {0};
    int Obj_ParentID {0};
    std::string TAG;
};

GUI parseGUI(const resource::Gff &gff);

} // namespace schema

} // namespace gui

} // namespace reone
