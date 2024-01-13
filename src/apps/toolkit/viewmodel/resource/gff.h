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

#include "reone/resource/gff.h"

#include "../../collectionproperty.h"
#include "../../property.h"

#include "../resource.h"

namespace reone {

namespace resource {

class TalkTable;

}

using GFFTreeNodeId = std::string;

enum class GFFTreeNodeType {
    Struct,
    Field,
    FieldComponent,
    StructField,
    ListField,
    ListItemStruct
};

struct GFFTreeNode {
    GFFTreeNodeId id;
    GFFTreeNodeType type;
    std::string displayName;
    std::optional<GFFTreeNodeId> parentId;

    GFFTreeNode(GFFTreeNodeId id,
                GFFTreeNodeType type,
                std::string displayName,
                std::optional<GFFTreeNodeId> parentId = std::nullopt) :
        id(id),
        type(type),
        displayName(std::move(displayName)),
        parentId(std::move(parentId)) {
    }
};

class GFFResourceViewModel : public ResourceViewModel {
public:
    GFFResourceViewModel(resource::TalkTable &talkTable,
                         std::shared_ptr<resource::Gff> content) :
        _talkTable(talkTable),
        _content(std::move(content)) {

        rebuildTreeFromGff();
    }

    resource::Gff &content() {
        return *_content;
    }

    CollectionProperty<std::shared_ptr<GFFTreeNode>> &treeNodes() {
        return _treeNodes;
    }

    const GFFTreeNode &treeNodeById(const GFFTreeNodeId &id) const {
        return _idToTreeNode.at(id);
    }

    const resource::Gff &gffByTreeNodeId(const GFFTreeNodeId &id) const {
        return _treeNodeIdToGff.at(id);
    }

    const resource::Gff::Field &fieldByTreeNodeId(const GFFTreeNodeId &id) const {
        return _treeNodeIdToField.at(id);
    }

    bool isContainerNode(const GFFTreeNodeId &id) const;

    void setStructType(const GFFTreeNodeId &id, uint32_t type);
    void appendField(const GFFTreeNodeId &id);
    void renameField(const GFFTreeNodeId &id, std::string name);
    void setFieldType(const GFFTreeNodeId &id, resource::Gff::FieldType type);
    void modifyField(const GFFTreeNodeId &id, std::function<void(resource::Gff::Field &)> block);
    void deleteField(const GFFTreeNodeId &id);
    void clearListItems(const GFFTreeNodeId &id);
    void appendListItem(const GFFTreeNodeId &id);
    void duplicateListItem(const GFFTreeNodeId &id);
    void deleteListItem(const GFFTreeNodeId &id);

private:
    resource::TalkTable &_talkTable;
    std::shared_ptr<resource::Gff> _content;

    CollectionProperty<std::shared_ptr<GFFTreeNode>> _treeNodes;

    std::unordered_map<GFFTreeNodeId, std::reference_wrapper<GFFTreeNode>> _idToTreeNode;
    std::unordered_map<GFFTreeNodeId, std::reference_wrapper<resource::Gff>> _treeNodeIdToGff;
    std::unordered_map<GFFTreeNodeId, std::reference_wrapper<resource::Gff::Field>> _treeNodeIdToField;
    std::unordered_map<GFFTreeNodeId, int> _treeNodeIdToListIdx;

    void rebuildTreeFromGff();
};

} // namespace reone
