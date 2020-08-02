#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

#ifdef REONE_WITH_TOOLS

#include <boost/property_tree/ptree.hpp>

#include "../resources/keyfile.h"

#else

#include "../resources/types.h"

#endif // REONE_WITH_TOOLS

namespace reone {

#ifdef REONE_WITH_TOOLS

namespace resources {

class GffStruct;

}

#endif // REONE_WITH_TOOLS

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

std::unique_ptr<Tool> getToolByPath(resources::GameVersion version, const boost::filesystem::path &path);

#ifdef REONE_WITH_TOOLS

class KeyTool : public Tool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
};

class BifTool : public Tool {
public:
    void list(const boost::filesystem::path &path, const boost::filesystem::path &keyPath) const override;
    void extract(const boost::filesystem::path &path, const boost::filesystem::path &keyPath, const boost::filesystem::path &destPath) const override;

private:
    int getFileIndexByFilename(const std::vector<resources::KeyFile::FileEntry> &files, const std::string &filename) const;
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

class GffTool : public Tool {
public:
    void convert(const boost::filesystem::path &path, const boost::filesystem::path &destPath) const override;

private:
    boost::property_tree::ptree getPropertyTree(const resources::GffStruct &gffs) const;
};

#endif // REONE_WITH_TOOLS

} // namespace tools

} // namespace reone
