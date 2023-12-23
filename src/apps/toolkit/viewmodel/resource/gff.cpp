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

#include "gff.h"

#include "reone/resource/talktable.h"
#include "reone/system/hexutil.h"

using namespace reone::resource;

namespace reone {

struct TreeBuilderContext {
    Gff &gff;
    GFFTreeNode &node;
    std::optional<std::reference_wrapper<GFFTreeNode>> parent;

    TreeBuilderContext(Gff &gff,
                       GFFTreeNode &node,
                       std::optional<std::reference_wrapper<GFFTreeNode>> parent = std::nullopt) :
        gff(gff),
        node(node),
        parent(std::move(parent)) {
    }
};

void GFFResourceViewModel::rebuildTreeFromGff() {
    std::list<std::shared_ptr<GFFTreeNode>> nodes;
    std::stack<TreeBuilderContext> stack;

    auto rootNode = std::make_shared<GFFTreeNode>("$", GFFTreeNodeType::Struct, str(boost::format("/ [%d]") % _content->type()));
    nodes.push_back(rootNode);
    stack.emplace(*_content, *rootNode);

    _treeNodeIdToGff.clear();
    _treeNodeIdToGff.insert({rootNode->id, *_content});
    _treeNodeIdToField.clear();
    _treeNodeIdToListIdx.clear();

    while (!stack.empty()) {
        auto ctx = stack.top();
        stack.pop();

        for (auto &field : ctx.gff.fields()) {
            auto fieldNodeId = str(boost::format("%s.%s") % ctx.node.id % field.label);
            _treeNodeIdToField.insert({fieldNodeId, field});
            switch (field.type) {
            case Gff::FieldType::Byte:
            case Gff::FieldType::Word:
            case Gff::FieldType::Dword: {
                auto fieldDisplayName = str(boost::format("%s [%d] = %d") % field.label % static_cast<int>(field.type) % field.uintValue);
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::Char:
            case Gff::FieldType::Short:
            case Gff::FieldType::Int:
            case Gff::FieldType::StrRef: {
                auto fieldDisplayName = str(boost::format("%s [%d] = %d") % field.label % static_cast<int>(field.type) % field.intValue);
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::Dword64: {
                auto fieldDisplayName = str(boost::format("%s [%d] = %llu") % field.label % static_cast<int>(field.type) % field.uint64Value);
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::Int64: {
                auto fieldDisplayName = str(boost::format("%s [%d] = %lld") % field.label % static_cast<int>(field.type) % field.int64Value);
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::Float: {
                auto fieldDisplayName = str(boost::format("%s [%d] = %f") % field.label % static_cast<int>(field.type) % field.floatValue);
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::Double: {
                auto fieldDisplayName = str(boost::format("%s [%d] = %f") % field.label % static_cast<int>(field.type) % field.doubleValue);
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::CExoString:
            case Gff::FieldType::ResRef: {
                auto fieldDisplayName = str(boost::format("%s [%d] = \"%s\"") % field.label % static_cast<int>(field.type) % boost::replace_all_copy(field.strValue, "\n", "\\n"));
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::CExoLocString: {
                auto fieldDisplayName = str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type));
                auto fieldNode = std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id);
                nodes.push_back(fieldNode);
                auto strRefNodeId = str(boost::format("%s.StrRef") % fieldNodeId);
                auto strRefDisplayName = str(boost::format("StrRef = %s") % field.intValue);
                nodes.push_back(std::make_shared<GFFTreeNode>(strRefNodeId, GFFTreeNodeType::FieldComponent, strRefDisplayName, fieldNodeId));
                auto substrNodeId = str(boost::format("%s.Substring") % fieldNodeId);
                auto substrDisplayName = str(boost::format("Substring = \"%s\"") % field.strValue);
                nodes.push_back(std::make_shared<GFFTreeNode>(substrNodeId, GFFTreeNodeType::FieldComponent, substrDisplayName, fieldNodeId));
                if (field.intValue != -1) {
                    auto talkTableTextNodeID = str(boost::format("%s.TalkTableText") % fieldNodeId);
                    auto text = _talkTable.getString(field.intValue).text;
                    auto talkTableTextDisplayName = str(boost::format("TalkTableText = \"%s\"") % text);
                    nodes.push_back(std::make_shared<GFFTreeNode>(talkTableTextNodeID, GFFTreeNodeType::FieldComponent, talkTableTextDisplayName, fieldNodeId));
                }
                break;
            }
            case Gff::FieldType::Void: {
                auto fieldDisplayName = str(boost::format("%s [%d] = \"%s\"") % field.label % static_cast<int>(field.type) % hexify(field.data, ""));
                nodes.push_back(std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id));
                break;
            }
            case Gff::FieldType::Orientation: {
                auto fieldDisplayName = str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type));
                auto fieldNode = std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id);
                nodes.push_back(fieldNode);
                auto wNodeId = str(boost::format("%s.W") % fieldNodeId);
                auto wDisplayName = str(boost::format("W = %f") % field.quatValue.w);
                nodes.push_back(std::make_shared<GFFTreeNode>(wNodeId, GFFTreeNodeType::FieldComponent, wDisplayName, fieldNodeId));
                auto xNodeId = str(boost::format("%s.X") % fieldNodeId);
                auto xDisplayName = str(boost::format("X = %f") % field.quatValue.x);
                nodes.push_back(std::make_shared<GFFTreeNode>(xNodeId, GFFTreeNodeType::FieldComponent, xDisplayName, fieldNodeId));
                auto yNodeId = str(boost::format("%s.Y") % fieldNodeId);
                auto yDisplayName = str(boost::format("Y = %f") % field.quatValue.y);
                nodes.push_back(std::make_shared<GFFTreeNode>(yNodeId, GFFTreeNodeType::FieldComponent, yDisplayName, fieldNodeId));
                auto zNodeId = str(boost::format("%s.Z") % fieldNodeId);
                auto zDisplayName = str(boost::format("Z = %f") % field.quatValue.z);
                nodes.push_back(std::make_shared<GFFTreeNode>(zNodeId, GFFTreeNodeType::FieldComponent, zDisplayName, fieldNodeId));
                break;
            }
            case Gff::FieldType::Vector: {
                auto fieldDisplayName = str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type));
                auto fieldNode = std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::Field, fieldDisplayName, ctx.node.id);
                nodes.push_back(fieldNode);
                auto xNodeId = str(boost::format("%s.X") % fieldNodeId);
                auto xDisplayName = str(boost::format("X = %f") % field.vecValue.x);
                nodes.push_back(std::make_shared<GFFTreeNode>(xNodeId, GFFTreeNodeType::FieldComponent, xDisplayName, fieldNodeId));
                auto yNodeId = str(boost::format("%s.Y") % fieldNodeId);
                auto yDisplayName = str(boost::format("Y = %f") % field.vecValue.y);
                nodes.push_back(std::make_shared<GFFTreeNode>(yNodeId, GFFTreeNodeType::FieldComponent, yDisplayName, fieldNodeId));
                auto zNodeId = str(boost::format("%s.Z") % fieldNodeId);
                auto zDisplayName = str(boost::format("Z = %f") % field.vecValue.z);
                nodes.push_back(std::make_shared<GFFTreeNode>(zNodeId, GFFTreeNodeType::FieldComponent, zDisplayName, fieldNodeId));
                break;
            }
            case Gff::FieldType::Struct: {
                auto fieldDisplayName = str(boost::format("%s [%d,%d]") % field.label % static_cast<int>(field.type) % field.children.front()->type());
                auto fieldNode = std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::StructField, fieldDisplayName, ctx.node.id);
                nodes.push_back(fieldNode);
                _treeNodeIdToGff.insert({fieldNodeId, *field.children.front()});
                stack.emplace(*field.children.front(), *fieldNode, ctx.node);
                break;
            }
            case Gff::FieldType::List: {
                auto fieldDisplayName = str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type));
                auto fieldNode = std::make_shared<GFFTreeNode>(fieldNodeId, GFFTreeNodeType::ListField, fieldDisplayName, ctx.node.id);
                nodes.push_back(fieldNode);
                for (size_t i = 0; i < field.children.size(); ++i) {
                    auto &child = field.children.at(i);
                    auto listItemNodeId = str(boost::format("%s[%d]") % fieldNodeId % i);
                    auto listItemDisplayName = str(boost::format("%d [%d]") % i % child->type());
                    auto listItemNode = std::make_shared<GFFTreeNode>(listItemNodeId, GFFTreeNodeType::ListItemStruct, listItemDisplayName, fieldNodeId);
                    nodes.push_back(listItemNode);
                    _treeNodeIdToGff.insert({listItemNodeId, *child});
                    _treeNodeIdToListIdx.insert({listItemNodeId, i});
                    stack.emplace(*child, *listItemNode, *fieldNode);
                }
                break;
            }
            }
        }
    }

    _idToTreeNode.clear();
    for (const auto &node : nodes) {
        _idToTreeNode.insert({node->id, *node});
    }

    _treeNodes = std::move(nodes);
}

void GFFResourceViewModel::setStructType(const GFFTreeNodeId &id, uint32_t type) {
    _treeNodeIdToGff.at(id).get().setType(type);
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::appendField(const GFFTreeNodeId &id) {
    auto &fields = _treeNodeIdToGff.at(id).get().fields();
    int numNewFields = 1;
    for (const auto &field : fields) {
        if (boost::starts_with(field.label, "New_Field")) {
            ++numNewFields;
        }
    }
    std::string label {"New_Field"};
    if (numNewFields > 1) {
        label += std::to_string(numNewFields);
    }
    auto newField = Gff::Field::newInt(label, 0);
    _treeNodeIdToGff.at(id).get().fields().push_back(std::move(newField));
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::renameField(const GFFTreeNodeId &id, std::string name) {
    _treeNodeIdToField.at(id).get().label = std::move(name);
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::setFieldType(const GFFTreeNodeId &id, Gff::FieldType type) {
    auto &field = _treeNodeIdToField.at(id).get();
    if (field.type == Gff::FieldType::Struct || field.type == Gff::FieldType::List) {
        field.children.clear();
    }
    field.type = type;
    if (field.type == Gff::FieldType::Struct) {
        field.children.push_back(Gff::Builder().build());
    }
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::modifyField(const GFFTreeNodeId &id, std::function<void(Gff::Field &)> block) {
    auto &field = _treeNodeIdToField.at(id).get();
    block(field);
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::deleteField(const GFFTreeNodeId &id) {
    auto &node = _idToTreeNode.at(id).get();
    auto &parentGff = _treeNodeIdToGff.at(*node.parentId).get();
    auto &field = _treeNodeIdToField.at(id).get();
    for (auto it = parentGff.fields().begin(); it != parentGff.fields().end(); ++it) {
        if (it->label == field.label) {
            parentGff.fields().erase(it);
            break;
        }
    }
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::appendListItem(const GFFTreeNodeId &id) {
    auto &node = _idToTreeNode.at(id).get();
    auto &field = _treeNodeIdToField.at(id).get();
    field.children.push_back(Gff::Builder().build());
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::duplicateListItem(const GFFTreeNodeId &id) {
    auto &node = _idToTreeNode.at(id).get();
    auto &parentField = _treeNodeIdToField.at(*node.parentId).get();
    auto iter = parentField.children.begin();
    auto itemIdx = _treeNodeIdToListIdx.at(id);
    std::advance(iter, itemIdx);
    auto &gff = _treeNodeIdToGff.at(id).get();
    auto copy = gff.deepCopy();
    parentField.children.insert(iter, std::move(copy));
    _modified = true;
    rebuildTreeFromGff();
}

void GFFResourceViewModel::deleteListItem(const GFFTreeNodeId &id) {
    auto &node = _idToTreeNode.at(id).get();
    auto &parentField = _treeNodeIdToField.at(*node.parentId).get();
    auto iter = parentField.children.begin();
    auto itemIdx = _treeNodeIdToListIdx.at(id);
    std::advance(iter, itemIdx);
    parentField.children.erase(iter);
    _modified = true;
    rebuildTreeFromGff();
}

bool GFFResourceViewModel::isContainerNode(const GFFTreeNodeId &id) const {
    auto &node = _idToTreeNode.at(id).get();
    switch (node.type) {
    case GFFTreeNodeType::Struct:
    case GFFTreeNodeType::StructField:
    case GFFTreeNodeType::ListField:
    case GFFTreeNodeType::ListItemStruct:
        return true;
    case GFFTreeNodeType::Field: {
        auto &field = _treeNodeIdToField.at(id).get();
        switch (field.type) {
        case Gff::FieldType::CExoLocString:
        case Gff::FieldType::Struct:
        case Gff::FieldType::List:
        case Gff::FieldType::Orientation:
        case Gff::FieldType::Vector:
            return true;
        default:
            return false;
        }
    }
    case GFFTreeNodeType::FieldComponent:
    default:
        return false;
    }
}

} // namespace reone
