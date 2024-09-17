#pragma once
#include <fstream>

namespace exif::details {

class reader {
public:
    explicit reader(std::fstream& ifs);

    template <typename T>
        requires std::is_integral_v<T>
    T read() {
        T value;
        stream_.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (is_big_endian_) {
            return value;
        }
        std::reverse(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(T));
        return value;
    }

    std::string read_until(char* tag, size_t size);

private:
    std::fstream& stream_;
    bool          is_big_endian_ = true;
};

} // namespace exif::details
