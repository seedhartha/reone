/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include <unordered_map>

#include <stdexcept>

#include "../common/log.h"

#include "types.h"

using namespace std;

namespace reone {

namespace resource {

static unordered_map<ResourceType, string> g_extByType {
    { ResourceType::Res, "inv" },
    { ResourceType::Bmp, "bmp" },
    { ResourceType::Tga, "tga" },
    { ResourceType::Wav, "wav" },
    { ResourceType::Plt, "plt" },
    { ResourceType::Ini, "ini" },
    { ResourceType::Txt, "txt" },
    { ResourceType::Mdl, "mdl" },
    { ResourceType::Nss, "nss" },
    { ResourceType::Ncs, "ncs" },
    { ResourceType::Are, "are" },
    { ResourceType::Set, "set" },
    { ResourceType::Ifo, "ifo" },
    { ResourceType::Bic, "bic" },
    { ResourceType::Wok, "wok" },
    { ResourceType::TwoDa, "2da" },
    { ResourceType::Txi, "txi" },
    { ResourceType::Git, "git" },
    { ResourceType::Bti, "bti" },
    { ResourceType::Uti, "uti" },
    { ResourceType::Btc, "btc" },
    { ResourceType::Utc, "utc" },
    { ResourceType::Dlg, "dlg" },
    { ResourceType::Itp, "itp" },
    { ResourceType::Utt, "utt" },
    { ResourceType::Dds, "dds" },
    { ResourceType::Uts, "uts" },
    { ResourceType::Ltr, "ltr" },
    { ResourceType::Gff, "gff" },
    { ResourceType::Fac, "fac" },
    { ResourceType::Ute, "ute" },
    { ResourceType::Utd, "utd" },
    { ResourceType::Utp, "utp" },
    { ResourceType::Dft, "dft" },
    { ResourceType::Gic, "gic" },
    { ResourceType::Gui, "gui" },
    { ResourceType::Utm, "utm" },
    { ResourceType::Dwk, "dwk" },
    { ResourceType::Pwk, "pwk" },
    { ResourceType::Jrl, "jrl" },
    { ResourceType::Mod, "mod" },
    { ResourceType::Utw, "utw" },
    { ResourceType::Ssf, "ssf" },
    { ResourceType::Ndb, "ndb" },
    { ResourceType::Ptm, "ptm" },
    { ResourceType::Ptt, "ptt" },
    { ResourceType::Lyt, "lyt" },
    { ResourceType::Vis, "vis" },
    { ResourceType::Pth, "pth" },
    { ResourceType::Lip, "lip" },
    { ResourceType::Tpc, "tpc" },
    { ResourceType::Mdx, "mdx" },
    { ResourceType::Mp3, "mp3" }
};

static unordered_map<string, ResourceType> g_typeByExt;
static bool g_typeByExtInited = false;

const string &getExtByResType(ResourceType type) {
    auto it = g_extByType.find(type);
    if (it != g_extByType.end()) return it->second;

    g_extByType.insert(make_pair(type, to_string(static_cast<int>(type))));

    return g_extByType[type];
}

ResourceType getResTypeByExt(const string &ext, bool logNotFound) {
    if (!g_typeByExtInited) {
        for (auto &entry : g_extByType) {
            g_typeByExt.insert(make_pair(entry.second, entry.first));
        }
        g_typeByExtInited = true;
    }
    auto it = g_typeByExt.find(ext);
    if (it == g_typeByExt.end()) {
        if (logNotFound) {
            warn("Resource type not found by extension: " + ext);
        }
        return ResourceType::Invalid;
    }

    return it->second;
}

} // namespace resource

} // namespace reone
