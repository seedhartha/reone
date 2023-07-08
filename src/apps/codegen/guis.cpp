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

#include "guis.h"

#include "reone/gui/types.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/provider/folder.h"
#include "reone/resource/provider/keybif.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/textwriter.h"

#include "templates.h"

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

struct ParsedControl {
    ControlType type {ControlType::Invalid};
    std::string tag;

    ParsedControl() = default;

    ParsedControl(ControlType type, std::string tag) :
        type(type), tag(tag) {
    }
};

struct ParsedGUI {
    std::map<std::string, ParsedControl> controls;
};

static ParsedGUI parseGui(ByteArray &guiBytes) {
    auto guiStream = MemoryInputStream(guiBytes);
    auto guiReader = GffReader(guiStream);
    guiReader.load();
    auto gff = guiReader.root();

    ParsedGUI parsed;
    auto controls = gff->getList("CONTROLS");
    for (auto &control : controls) {
        auto type = control->getEnum("CONTROLTYPE", ControlType::Invalid);
        auto tag = control->getString("TAG");
        parsed.controls[tag] = ParsedControl(type, tag);
    }
    return parsed;
}

static ParsedGUI mergeGuis(const ParsedGUI &k1Gui, const ParsedGUI &k2Gui) {
    ParsedGUI merged;
    for (auto &[tag, control] : k1Gui.controls) {
        merged.controls[tag] = control;
    }
    for (auto &[tag, control] : k2Gui.controls) {
        if (merged.controls.count(tag) > 0) {
            auto &exisiting = merged.controls.at(tag);
            if (control.type != exisiting.type) {
                throw std::runtime_error(str(boost::format("Control type mismatch: %s, %d, %d") % tag % static_cast<int>(control.type) % static_cast<int>(exisiting.type)));
            }
            continue;
        }
        merged.controls[tag] = control;
    }
    return merged;
}

static void writeControlDeclaration(const std::string &typeName,
                                    const std::string &tag,
                                    TextWriter &writer) {
    writer.put(str(boost::format("std::shared_ptr<%s> %s;\n") % typeName % tag));
}

static std::string controlTypeToTypeName(ControlType type) {
    std::string typeName;
    switch (type) {
    case ControlType::Panel:
        typeName = "gui::Panel";
        break;
    case ControlType::Label:
        typeName = "gui::Label";
        break;
    case ControlType::LabelHilight:
        typeName = "gui::LabelHilight";
        break;
    case ControlType::Button:
        typeName = "gui::Button";
        break;
    case ControlType::ButtonToggle:
        typeName = "gui::ButtonToggle";
        break;
    case ControlType::Slider:
        typeName = "gui::Slider";
        break;
    case ControlType::ScrollBar:
        typeName = "gui::ScrollBar";
        break;
    case ControlType::ProgressBar:
        typeName = "gui::ProgressBar";
        break;
    case ControlType::ListBox:
        typeName = "gui::ListBox";
        break;
    default:
        throw std::runtime_error("Invalid control type: " + std::to_string(static_cast<int>(type)));
    }
    return typeName;
}

static void writeHeaderFile(const std::string &resRef,
                            const ParsedGUI &gui,
                            const boost::filesystem::path &destDir) {
    auto path = destDir;
    path.append(resRef + ".h");
    auto stream = FileOutputStream(path);
    auto writer = TextWriter(stream);
    writer.put(kCopyrightNotice + "\n\n");
    writer.put("#pragma once\n\n");
    writer.put(str(boost::format(kIncludeFormat + "\n\n") % "reone/gui/gui.h"));
    writer.put("namespace reone {\n\n");
    writer.put("namespace game {\n\n");
    writer.put(str(boost::format("class GUI_%s : gui::IGUI, boost::noncopyable {\n") % resRef));
    writer.put("public:\n");
    writer.put(kIndent + "void bindControls() {\n");
    for (auto &[tag, control] : gui.controls) {
        auto typeName = controlTypeToTypeName(control.type);
        writer.put(str(boost::format("%1%%1%_controls.%2% = findControl<%3%>(\"%2%\");\n") % kIndent % tag % typeName));
    }
    writer.put(kIndent + "}\n");
    writer.put("\n");
    writer.put("private:\n");
    writer.put(kIndent + "struct Controls {\n");
    for (auto &[tag, control] : gui.controls) {
        writer.put(kIndent);
        writer.put(kIndent);
        auto typeName = controlTypeToTypeName(control.type);
        writeControlDeclaration(typeName, tag, writer);
    }
    writer.put(kIndent + "};\n");
    writer.put("\n");
    writer.put(kIndent + "Controls _controls;\n");
    writer.put("};\n\n");
    writer.put("} // namespace game\n\n");
    writer.put("} // namespace reone\n");
}

void generateGuis(const boost::filesystem::path &k1dir,
                  const boost::filesystem::path &k2dir,
                  const boost::filesystem::path &destDir) {
    std::set<std::string> guiResRefs;

    auto k1KeyPath = findFileIgnoreCase(k1dir, "chitin.key");
    auto k1KeyBifProvider = KeyBifResourceProvider(k1KeyPath);
    k1KeyBifProvider.init();
    for (auto &[resId, _] : k1KeyBifProvider.resources()) {
        if (resId.type == ResourceType::Gui) {
            guiResRefs.insert(resId.resRef);
        }
    }

    auto k2KeyPath = findFileIgnoreCase(k2dir, "chitin.key");
    auto k2KeyBifProvider = KeyBifResourceProvider(k2KeyPath);
    k2KeyBifProvider.init();
    for (auto &[resId, _] : k2KeyBifProvider.resources()) {
        if (resId.type != ResourceType::Gui) {
            continue;
        }
        if (!boost::ends_with(resId.resRef, "_x") && !boost::ends_with(resId.resRef, "_p")) {
            throw std::runtime_error("Invalid TSL GUI ResRef");
        }
        auto strippedResRef = resId.resRef.substr(0, resId.resRef.length() - 2);
        guiResRefs.insert(std::move(strippedResRef));
    }

    auto k2OverridePath = findFileIgnoreCase(k2dir, "override");
    auto k2OverrideFolder = Folder(k2OverridePath);
    k2OverrideFolder.init();
    for (auto &[resRef, res] : k2OverrideFolder.resources()) {
        if (res.type != ResourceType::Gui) {
            continue;
        }
        if (!boost::ends_with(resRef, "_x") && !boost::ends_with(resRef, "_p")) {
            throw std::runtime_error("Invalid TSL GUI ResRef");
        }
        auto strippedResRef = resRef.substr(0, resRef.length() - 2);
        guiResRefs.insert(std::move(strippedResRef));
    }

    auto k1Providers = std::vector<IResourceProvider *> {&k1KeyBifProvider};
    auto k2Providers = std::vector<IResourceProvider *> {&k2OverrideFolder, &k2KeyBifProvider};
    for (auto &resRef : guiResRefs) {
        std::unique_ptr<ParsedGUI> k1Gui;
        for (auto &provider : k1Providers) {
            auto bytes = provider->find(ResourceId(resRef, ResourceType::Gui));
            if (bytes) {
                k1Gui = std::make_unique<ParsedGUI>(parseGui(*bytes));
                break;
            }
        }

        std::unique_ptr<ParsedGUI> k2Gui;
        for (auto &provider : k2Providers) {
            auto bytes = provider->find(ResourceId(resRef + "_p", ResourceType::Gui));
            if (bytes) {
                k2Gui = std::make_unique<ParsedGUI>(parseGui(*bytes));
                break;
            }
        }

        ParsedGUI mergedGui;
        if (k1Gui && k2Gui) {
            mergedGui = mergeGuis(*k1Gui, *k2Gui);
        } else if (k1Gui) {
            mergedGui = *k1Gui;
        } else if (k2Gui) {
            mergedGui = *k2Gui;
        }

        writeHeaderFile(resRef, mergedGui, destDir);
    }
}

} // namespace reone
