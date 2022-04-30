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

#include "gff.h"

#include "../../common/hexutil.h"
#include "../../common/streamwriter.h"
#include "../../resource/format/gffreader.h"
#include "../../resource/format/gffwriter.h"
#include "../../resource/typeutil.h"

#include "tinyxml2.h"

using namespace std;

using namespace tinyxml2;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void GffTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    switch (operation) {
    case Operation::ToXML:
        toXML(target, destPath);
        break;
    case Operation::ToGFF:
        toGFF(target, destPath);
        break;
    default:
        break;
    }
}

static string sanitizeXmlElementName(const std::string &s) {
    auto sanitized = s;
    boost::replace_all(sanitized, " ", "_");
    return move(sanitized);
}

static void printStructToXml(const Gff &gff, XMLPrinter &printer, int index = -1) {
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
        case Gff::FieldType::CExoLocString:
            printer.PushAttribute("strref", field.intValue);
            printer.PushAttribute("substring", field.strValue.c_str());
            break;
        case Gff::FieldType::Void:
            printer.PushAttribute("data", hexify(field.data, "").c_str());
            break;
        case Gff::FieldType::Struct:
            printStructToXml(*field.children[0], printer);
            break;
        case Gff::FieldType::List:
            for (size_t i = 0; i < field.children.size(); ++i) {
                printStructToXml(*field.children[i], printer, i);
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

void GffTool::toXML(const fs::path &path, const fs::path &destPath) {
    auto reader = GffReader();
    reader.load(path);

    auto gff = reader.root();

    auto xmlPath = destPath;
    xmlPath.append(path.filename().string() + ".xml");
    auto fp = fopen(xmlPath.string().c_str(), "wb");

    auto printer = XMLPrinter(fp);
    printer.PushHeader(false, true);
    printStructToXml(*gff, printer);

    fclose(fp);
}

static unique_ptr<Gff> elementToGff(const XMLElement &element) {
    if (strncmp(element.Name(), "struct", 6) != 0) {
        throw invalid_argument("XML element must have name 'struct'");
    }

    auto structType = element.UnsignedAttribute("type");

    auto gff = Gff::Builder();
    gff.type(structType);

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
            throw logic_error("Unsupported field type: " + to_string(static_cast<int>(fieldType)));
        }

        gff.field(move(field));
    }

    return gff.build();
}

static void convertXmlToGff(const fs::path &path, const fs::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    fclose(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        cerr << "XML is empty" << endl;
        return;
    }

    auto extensionless = path;
    extensionless.replace_extension();
    auto resType = getResTypeByExt(extensionless.extension().string().substr(1));

    auto gffPath = destPath;
    gffPath.append(extensionless.filename().string());

    auto writer = GffWriter(resType, elementToGff(*rootElement));
    writer.save(gffPath);
}

void GffTool::toGFF(const fs::path &path, const fs::path &destPath) {
    if (path.extension() == ".xml") {
        convertXmlToGff(path, destPath);
    } else {
        cerr << "Input file must have XML extension" << endl;
    }
}

bool GffTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           (operation == Operation::ToXML || operation == Operation::ToGFF);
}

} // namespace reone
