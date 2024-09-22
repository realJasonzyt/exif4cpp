#pragma once
#include <fstream>

namespace exif::details {

class Reader {
public:
    explicit Reader(std::fstream& ifs);

    template <typename T>
        requires std::is_integral_v<T>
    T read() {
        T value;
        stream_.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (platform_big_endian_ == file_big_endian_) {
            return value;
        }
        std::reverse(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(T));
        return value;
    }

    std::string read(size_t size);

    template <typename T>
        requires std::is_integral_v<T>
    void read(T& dest) {
        dest = read<T>();
    }

    std::string readUntil(char* tag, size_t size);

    void skip(size_t size);

    void setFileEndian(bool big_endian);

private:
    std::fstream& stream_;
    bool          platform_big_endian_ = true;
    bool          file_big_endian_     = true;
};

} // namespace exif::details
