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

#include "reone/tools/gff.h"

#include "tinyxml2.h"

#include "reone/resource/exception/format.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/format/gffwriter.h"
#include "reone/resource/typeutil.h"
#include "reone/system/binarywriter.h"
#include "reone/system/fileutil.h"
#include "reone/system/hexutil.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

using namespace tinyxml2;

using namespace reone::resource;

namespace reone {

void GffTool::invoke(Operation operation,
                     const boost::filesystem::path &input,
                     const boost::filesystem::path &outputDir,
                     const boost::filesystem::path &gamePath) {

    return invokeBatch(operation, std::vector<boost::filesystem::path> {input}, outputDir, gamePath);
}

void GffTool::invokeBatch(
    Operation operation,
    const std::vector<boost::filesystem::path> &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        switch (operation) {
        case Operation::ToXML:
            toXML(path, outDir);
            break;
        case Operation::ToGFF:
            toGFF(path, outDir);
            break;
        default:
            break;
        }
    });
}

static std::string sanitizeXmlElementName(const std::string &s) {
    auto sanitized = s;
    boost::replace_all(sanitized, " ", "_");
    return std::move(sanitized);
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
        case Gff::FieldType::CExoLocString: {
            printer.PushAttribute("strref", field.intValue);
            printer.PushAttribute("substring", field.strValue.c_str());
            break;
        }
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

void GffTool::toXML(const boost::filesystem::path &input, const boost::filesystem::path &outputDir) {
    auto gff = FileInputStream(input);

    auto xmlPath = outputDir;
    xmlPath.append(input.filename().string() + ".xml");
    auto xml = FileOutputStream(xmlPath);

    toXML(gff, xml);
}

void GffTool::toXML(IInputStream &gff, IOutputStream &xml) {
    auto reader = GffReader(gff);
    reader.load();

    auto gffStruct = reader.root();

    auto printer = XMLPrinter();
    printer.PushHeader(false, true);
    printStructToXml(*gffStruct, printer);

    xml.write(printer.CStr(), printer.CStrSize() - 1);
}

static std::unique_ptr<Gff> elementToGff(const XMLElement &element) {
    if (strncmp(element.Name(), "struct", 6) != 0) {
        throw FormatException("XML element must have name 'struct'");
    }

    auto structType = element.UnsignedAttribute("type");
    auto fields = std::vector<Gff::Field>();

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
            throw FormatException("Unsupported field type: " + std::to_string(static_cast<int>(fieldType)));
        }

        fields.push_back(std::move(field));
    }

    return std::make_unique<Gff>(structType, std::move(fields));
}

static void convertXmlToGff(const boost::filesystem::path &input, const boost::filesystem::path &outputDir) {
    auto fp = fopen(input.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    fclose(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        std::cerr << "XML is empty" << std::endl;
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

void GffTool::toGFF(const boost::filesystem::path &input, const boost::filesystem::path &outputDir) {
    if (input.extension() == ".xml") {
        convertXmlToGff(input, outputDir);
    } else {
        std::cerr << "Input file must have XML extension" << std::endl;
    }
}

bool GffTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           (operation == Operation::ToXML || operation == Operation::ToGFF);
}

} // namespace reone
