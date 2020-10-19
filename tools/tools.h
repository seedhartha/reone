/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../src/system/resource/collection/keyfile.h"

namespace reone {

namespace resource {

class GffStruct;

}

namespace tools {

/**
 * Abstract tool class. Each implementation covers a single file format.
 *
 * Operations:
 * - list — list file contents
 * - extract — extract file contents
 * - convert — convert file to JSON
 */
class Tool {
public:
    virtual void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const;
    virtual void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const;
    virtual void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const;

private:
    void throwNotImplemented() const;
};

std::unique_ptr<Tool> getToolByPath(resource::GameVersion version, const boost::filesystem::path &path);

class KeyTool : public Tool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
};

class BifTool : public Tool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;

private:
    int getFileIndexByFilename(const std::vector<resource::KeyFile::FileEntry> &files, const std::string &filename) const;
};

class ErfTool : public Tool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;
};

class RimTool : public Tool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;
};

class TwoDaTool : public Tool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;
};

class TlkTool : public Tool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;
};

class GffTool : public Tool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;

private:
    boost::property_tree::ptree getPropertyTree(const resource::GffStruct &gffs) const;
};

} // namespace tools

} // namespace reone
