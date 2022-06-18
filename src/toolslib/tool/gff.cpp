/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "gff.h"

#include "../../common/binarywriter.h"
#include "../../common/exception/validation.h"
#include "../../common/hexutil.h"
#include "../../common/logutil.h"
#include "../../common/pathutil.h"
#include "../../common/stream/fileinput.h"
#include "../../resource/format/gffreader.h"
#include "../../resource/format/gffwriter.h"
#include "../../resource/strings.h"
#include "../../resource/typeutil.h"

#include "tinyxml2.h"

using namespace std;

using namespace tinyxml2;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void GffTool::invoke(Operation operation,
                     const fs::path &input,
                     const fs::path &outputDir,
                     const fs::path &gamePath) {

    return invokeAll(operation, vector<fs::path> {input}, outputDir, gamePath);
}

void GffTool::invokeAll(
    Operation operation,
    const vector<fs::path> &input,
    const fs::path &outputDir,
    const fs::path &gamePath) {

    auto strings = Strings();
    if (!gamePath.empty()) {
        auto tlkPath = getPathIgnoreCase(gamePath, "dialog.tlk", false);
        if (!tlkPath.empty()) {
            strings.init(gamePath);
        }
    }
    for (auto &path : input) {
        auto outDir = outputDir;
        if (outDir.empty()) {
            outDir = path.parent_path();
        }
        try {
            switch (operation) {
            case Operation::ToXML:
                toXML(path, outDir, strings);
                break;
            case Operation::ToGFF:
                toGFF(path, outDir);
                break;
            default:
                break;
            }
        } catch (const ValidationException &e) {
            error(boost::format("Error while processing '%s': %s") % path % string(e.what()));
        }
    }
}

static string sanitizeXmlElementName(const std::string &s) {
    auto sanitized = s;
    boost::replace_all(sanitized, " ", "_");
    return move(sanitized);
}

static void printStructToXml(const Gff &gff, XMLPrinter &printer, Strings &strings, int index = -1) {
    printer.OpenElement("struct");
    printer.PushAttribute("type", gff.type());
    if (index != -1) {
        printer.PushAttribute("index", index);
    }

    for (auto &field : gff.fields()) {
        auto sanitizedName = sanitizeXmlElementName(field.label);
        printer.OpenElement(sanitizedName.c_str());
        if (sanitizedName != field.label) {
            printer.PushAttribute("name", field.label.c_str());
        }

        printer.PushAttribute("type", static_cast<int>(field.type));

        switch (field.type) {
        case Gff::FieldType::CExoLocString: {
            printer.PushAttribute("strref", field.intValue);
            printer.PushAttribute("substring", field.strValue.c_str());
            auto external = strings.get(field.intValue);
            if (!external.empty()) {
                printer.PushAttribute("external", external.c_str());
            }
            break;
        }
        case Gff::FieldType::Void:
            printer.PushAttribute("data", hexify(field.data, "").c_str());
            break;
        case Gff::FieldType::Struct:
            printStructToXml(*field.children[0], printer, strings);
            break;
        case Gff::FieldType::List:
            for (size_t i = 0; i < field.children.size(); ++i) {
                printStructToXml(*field.children[i], printer, strings, i);
            }
            break;
        case Gff::FieldType::Orientation:
            printer.PushAttribute("w", field.quatValue.w);
            printer.PushAttribute("x", field.quatValue.x);
            printer.PushAttribute("y", field.quatValue.y);
            printer.PushAttribute("z", field.quatValue.z);
            break;
        case Gff::FieldType::Vector:
            printer.PushAttribute("x", field.vecValue.x);
            printer.PushAttribute("y", field.vecValue.y);
            printer.PushAttribute("z", field.vecValue.z);
            break;
        default:
            printer.PushAttribute("value", field.toString().c_str());
            break;
        }

        printer.CloseElement();
    }

    printer.CloseElement();
}

void GffTool::toXML(const fs::path &input, const fs::path &outputDir, Strings &strings) {
    auto stream = FileInputStream(input, OpenMode::Binary);

    auto reader = GffReader();
    reader.load(stream);

    auto gff = reader.root();

    auto xmlPath = outputDir;
    xmlPath.append(input.filename().string() + ".xml");
    auto fp = fopen(xmlPath.string().c_str(), "wb");

    auto printer = XMLPrinter(fp);
    printer.PushHeader(false, true);
    printStructToXml(*gff, printer, strings);

    fclose(fp);
}

static unique_ptr<Gff> elementToGff(const XMLElement &element) {
    if (strncmp(element.Name(), "struct", 6) != 0) {
        throw invalid_argument("XML element must have name 'struct'");
    }

    auto structType = element.UnsignedAttribute("type");
    auto fields = vector<Gff::Field>();

    for (auto fieldElement = element.FirstChildElement(); fieldElement; fieldElement = fieldElement->NextSiblingElement()) {
        auto fieldType = static_cast<Gff::FieldType>(fieldElement->IntAttribute("type"));
        auto maybeName = fieldElement->Attribute("name");
        auto fieldName = maybeName ? maybeName : fieldElement->Name();
        auto field = Gff::Field(fieldType, fieldName);

        switch (fieldType) {
        case Gff::FieldType::Byte:
        case Gff::FieldType::Word:
        case Gff::FieldType::Dword:
            field.uintValue = fieldElement->UnsignedAttribute("value");
            break;
        case Gff::FieldType::Char:
        case Gff::FieldType::Short:
        case Gff::FieldType::Int:
            field.intValue = fieldElement->IntAttribute("value");
            break;
        case Gff::FieldType::Dword64:
            field.uint64Value = fieldElement->Unsigned64Attribute("value");
            break;
        case Gff::FieldType::Int64:
            field.int64Value = fieldElement->Int64Attribute("value");
            break;
        case Gff::FieldType::Float:
            field.floatValue = fieldElement->FloatAttribute("value");
            break;
        case Gff::FieldType::Double:
            field.doubleValue = fieldElement->DoubleAttribute("value");
            break;
        case Gff::FieldType::CExoString:
        case Gff::FieldType::ResRef:
            field.strValue = fieldElement->Attribute("value");
            break;
        case Gff::FieldType::CExoLocString:
            field.intValue = fieldElement->IntAttribute("strref");
            field.strValue = fieldElement->Attribute("substring");
            break;
        case Gff::FieldType::Void:
            field.data = unhexify(fieldElement->Attribute("data"));
            break;
        case Gff::FieldType::Struct:
            field.children.push_back(elementToGff(*fieldElement->FirstChildElement()));
            break;
        case Gff::FieldType::List:
            for (auto listElement = fieldElement->FirstChildElement(); listElement; listElement = listElement->NextSiblingElement()) {
                field.children.push_back(elementToGff(*listElement));
            }
            break;
        case Gff::FieldType::Orientation:
            field.quatValue = glm::quat(
                fieldElement->FloatAttribute("w"),
                fieldElement->FloatAttribute("x"),
                fieldElement->FloatAttribute("y"),
                fieldElement->FloatAttribute("z"));
            break;
        case Gff::FieldType::Vector:
            field.vecValue = glm::vec3(
                fieldElement->FloatAttribute("x"),
                fieldElement->FloatAttribute("y"),
                fieldElement->FloatAttribute("z"));
            break;
        default:
            throw ValidationException("Unsupported field type: " + to_string(static_cast<int>(fieldType)));
        }

        fields.push_back(move(field));
    }

    return make_unique<Gff>(structType, move(fields));
}

static void convertXmlToGff(const fs::path &input, const fs::path &outputDir) {
    auto fp = fopen(input.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    fclose(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        cerr << "XML is empty" << endl;
        return;
    }

    auto extensionless = input;
    extensionless.replace_extension();
    auto resType = getResTypeByExt(extensionless.extension().string().substr(1));

    auto gffPath = outputDir;
    gffPath.append(extensionless.filename().string());

    auto writer = GffWriter(resType, elementToGff(*rootElement));
    writer.save(gffPath);
}

void GffTool::toGFF(const fs::path &input, const fs::path &outputDir) {
    if (input.extension() == ".xml") {
        convertXmlToGff(input, outputDir);
    } else {
        cerr << "Input file must have XML extension" << endl;
    }
}

bool GffTool::supports(Operation operation, const fs::path &input) const {
    return !fs::is_directory(input) &&
           (operation == Operation::ToXML || operation == Operation::ToGFF);
}

} // namespace reone
