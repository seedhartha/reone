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

#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../src/resource/keyfile.h"

namespace reone {

namespace resource {

class GffStruct;

}

namespace tools {

/**
 * Abstract tool, operating on a single file. Each implementation covers a
 * particular file format.
 *
 * Operations:
 * - list — list file contents
 * - extract — extract file contents
 * - convert — convert file to a more practical format, e.g. JSON, TGA
 */
class FileTool {
public:
    virtual void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const;
    virtual void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const;
    virtual void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const;

private:
    void throwNotImplemented() const;
};

std::unique_ptr<FileTool> getFileToolByPath(resource::GameVersion version, const boost::filesystem::path &path);

class KeyTool : public FileTool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
};

class BifTool : public FileTool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;

private:
    int getFileIndexByFilename(const std::vector<resource::KeyFile::FileEntry> &files, const std::string &filename) const;
};

class ErfTool : public FileTool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;
};

class RimTool : public FileTool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;
};

class TwoDaTool : public FileTool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;
};

class TlkTool : public FileTool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;
};

class GffTool : public FileTool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;

private:
    boost::property_tree::ptree getPropertyTree(const resource::GffStruct &gffs) const;
};

class TpcTool : public FileTool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;
};

} // namespace tools

} // namespace reone
