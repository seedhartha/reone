#pragma once

#include <boost/filesystem.hpp>

#include "../core/types.h"

namespace reone {

namespace resources {

/**
 * Abstract class with utility methods for reading binary files.
 */
class BinaryFile {
public:
    void load(const std::shared_ptr<std::istream> &in);
    void load(const boost::filesystem::path &path);

protected:
    boost::filesystem::path _path;
    std::shared_ptr<std::istream> _in;
    size_t _size { 0 };

    template <typename T>
    static void seek(std::istream &in, T off) {
        in.seekg(off);
    }

    template <typename T>
    static std::vector<T> readArray(std::istream &in, int n) {
        if (n == 0) return std::vector<T>();

        std::vector<T> arr(n);
        in.read(reinterpret_cast<char *>(&arr[0]), n * sizeof(T));

        return std::move(arr);
    }

    static inline std::vector<char> readArray(std::istream &in, int n);

    template <typename T>
    static std::vector<T> readArray(std::istream &in, uint32_t off, int n) {
        std::streampos pos = in.tellg();
        in.seekg(off);

        std::vector<T> arr(readArray<T>(in, n));
        in.seekg(pos);

        return std::move(arr);
    }

    static inline std::vector<char> readArray(std::istream &in, uint32_t off, int n);

    BinaryFile(int signSize, const char *sign = 0);

    virtual void doLoad() = 0;
    uint32_t tell() const;
    void ignore(int size);
    uint8_t readByte();
    int16_t readInt16();
    uint16_t readUint16();
    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();
    float readFloat();
    double readDouble();
    std::string readFixedString(int size);
    std::string readFixedString(uint32_t off, int size);
    std::string readString(uint32_t off);
    std::string readString(uint32_t off, int size);

    template <typename T>
    void seek(T off) {
        _in->seekg(off);
    }

    template <typename T>
    std::vector<T> readArray(int n) {
        return readArray<T>(*_in, n);
    }

    template <typename T>
    std::vector<T> readArray(uint32_t off, int n) {
        return readArray<T>(*_in, off, n);
    }

private:
    int _signSize { 0 };
    ByteArray _sign;

    BinaryFile(const BinaryFile &) = delete;
    BinaryFile &operator=(const BinaryFile &) = delete;

    void load();
    void querySize();
    void checkSignature();
};

} // namespace resources

} // namespace reone
