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

#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../src/resource/format/2dafile.h"
#include "../src/resource/format/gfffile.h"
#include "../src/resource/keybifprovider.h"
#include "../src/resource/format/rimfile.h"
#include "../src/resource/format/tlkfile.h"

namespace reone {

namespace tools {

/**
 * Tool to process a game module and produce a JSON file describing it.
 */
class ModuleProbe {
public:
    /**
     * Finds a module with the specified name in the specified game path,
     * processes it and produces a JSON file in the specified destination
     * directory, describing it. 
     */
    void probe(const std::string &name, const boost::filesystem::path &gamePath, const boost::filesystem::path &destPath);

private:
    boost::filesystem::path _gamePath;
    resource::KeyBifResourceProvider _keyBifProvider;
    std::shared_ptr<resource::TalkTable> _talkTable;
    resource::RimFile _rimMain;
    resource::RimFile _rimBlueprints;
    std::unordered_map<std::string, std::shared_ptr<ByteArray>> _resourceCache;

    void loadResources(const std::string &moduleName);
    boost::property_tree::ptree describeModule();
    boost::property_tree::ptree describeArea(const std::string &name, const resource::TwoDaTable &appearance, const resource::TwoDaTable &placeables);
    boost::property_tree::ptree describeCreature(const resource::GffStruct &gitCreature, const resource::TwoDaTable &appearance);
    boost::property_tree::ptree describeDoor(const resource::GffStruct &gitDoor);
    boost::property_tree::ptree describePlaceable(const resource::GffStruct &gitPlaceable, const resource::TwoDaTable &placeables);
    boost::property_tree::ptree describeTrigger(const resource::GffStruct &gitTrigger);
    boost::property_tree::ptree describeWaypoint(const resource::GffStruct &gitWaypoint);
    boost::property_tree::ptree describeSound(const resource::GffStruct &gitSound);
    void writeDescription(const std::string &moduleName, const boost::property_tree::ptree &tree, const boost::filesystem::path &destPath);

    std::shared_ptr<ByteArray> getResource(const std::string &resRef, resource::ResourceType type);
    std::string getString(int strRef) const;
};

} // namespace tools

} // namespace reone
