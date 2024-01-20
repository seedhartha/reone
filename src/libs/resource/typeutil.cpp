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

#include "reone/system/logutil.h"

#include "reone/resource/types.h"

namespace reone {

namespace resource {

static std::unordered_map<ResType, std::string> g_extByType {
    {ResType::Res, "res"},
    {ResType::Bmp, "bmp"},
    {ResType::Tga, "tga"},
    {ResType::Wav, "wav"},
    {ResType::Plt, "plt"},
    {ResType::Ini, "ini"},
    {ResType::Txt, "txt"},
    {ResType::Mdl, "mdl"},
    {ResType::Nss, "nss"},
    {ResType::Ncs, "ncs"},
    {ResType::Are, "are"},
    {ResType::Set, "set"},
    {ResType::Ifo, "ifo"},
    {ResType::Bic, "bic"},
    {ResType::Wok, "wok"},
    {ResType::TwoDA, "2da"},
    {ResType::Tlk, "tlk"},
    {ResType::Txi, "txi"},
    {ResType::Git, "git"},
    {ResType::Bti, "bti"},
    {ResType::Uti, "uti"},
    {ResType::Btc, "btc"},
    {ResType::Utc, "utc"},
    {ResType::Dlg, "dlg"},
    {ResType::Itp, "itp"},
    {ResType::Utt, "utt"},
    {ResType::Dds, "dds"},
    {ResType::Uts, "uts"},
    {ResType::Ltr, "ltr"},
    {ResType::Gff, "gff"},
    {ResType::Fac, "fac"},
    {ResType::Ute, "ute"},
    {ResType::Utd, "utd"},
    {ResType::Utp, "utp"},
    {ResType::Dft, "dft"},
    {ResType::Gic, "gic"},
    {ResType::Gui, "gui"},
    {ResType::Utm, "utm"},
    {ResType::Dwk, "dwk"},
    {ResType::Pwk, "pwk"},
    {ResType::Jrl, "jrl"},
    {ResType::Mod, "mod"},
    {ResType::Utw, "utw"},
    {ResType::Ssf, "ssf"},
    {ResType::Ndb, "ndb"},
    {ResType::Ptm, "ptm"},
    {ResType::Ptt, "ptt"},
    {ResType::Lyt, "lyt"},
    {ResType::Vis, "vis"},
    {ResType::Pth, "pth"},
    {ResType::Lip, "lip"},
    {ResType::Tpc, "tpc"},
    {ResType::Mdx, "mdx"},
    {ResType::Mp3, "mp3"},
    {ResType::Glsl, "glsl"}};

static std::unordered_map<std::string, ResType> g_typeByExt;
static bool g_typeByExtInited = false;

const std::string &getExtByResType(ResType type) {
    auto it = g_extByType.find(type);
    if (it != g_extByType.end())
        return it->second;

    g_extByType.insert(std::make_pair(type, std::to_string(static_cast<int>(type))));

    return g_extByType[type];
}

ResType getResTypeByExt(const std::string &ext, bool logNotFound) {
    if (!g_typeByExtInited) {
        for (auto &entry : g_extByType) {
            g_typeByExt.insert(std::make_pair(entry.second, entry.first));
        }
        g_typeByExtInited = true;
    }
    auto it = g_typeByExt.find(ext);
    if (it == g_typeByExt.end()) {
        if (logNotFound) {
            warn("Resource type not found by extension: " + ext);
        }
        return ResType::Invalid;
    }

    return it->second;
}

bool isGFFCompatibleResType(ResType type) {
    switch (type) {
    case ResType::Res:
    case ResType::Plt:
    case ResType::Are:
    case ResType::Set:
    case ResType::Ifo:
    case ResType::Bic:
    case ResType::Git:
    case ResType::Bti:
    case ResType::Uti:
    case ResType::Btc:
    case ResType::Utc:
    case ResType::Dlg:
    case ResType::Itp:
    case ResType::Utt:
    case ResType::Uts:
    case ResType::Gff:
    case ResType::Fac:
    case ResType::Ute:
    case ResType::Utd:
    case ResType::Utp:
    case ResType::Dft:
    case ResType::Gic:
    case ResType::Gui:
    case ResType::Utm:
    case ResType::Dwk:
    case ResType::Pwk:
    case ResType::Jrl:
    case ResType::Utw:
    case ResType::Ptm:
    case ResType::Ptt:
    case ResType::Pth:
        return true;
    default:
        return false;
    }
}

} // namespace resource

} // namespace reone
