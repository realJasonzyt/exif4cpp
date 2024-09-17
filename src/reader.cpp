#include "reader.h"
#include <cstring>

namespace exif::details {

bool isBigEndian() {
    union {
        uint32_t i;
        char     c[4];
    } u;
    u.i = 0x01020304;
    return u.c[0] == 1;
}

Reader::Reader(std::fstream& ifs) : stream_(ifs), platform_big_endian_(isBigEndian()) {}

std::string Reader::read(size_t size) {
    std::string result;
    result.resize(size);
    stream_.read(result.data(), size);
    return result;
}

std::string Reader::readUntil(char* tag, size_t size) {
    std::string result;
    result.resize(size);
    stream_.read(result.data(), size);
    while (result.size() >= size && std::memcmp(result.data() + result.size() - size, tag, size) != 0 && stream_.good()
    ) {
        result.resize(result.size() + 1);
        stream_.read(result.data() + result.size() - 1, 1);
    }
    return result;
}

void Reader::skip(size_t size) { stream_.seekg(size, std::ios::cur); }

void Reader::setFileEndian(bool big_endian) { file_big_endian_ = big_endian; }

} // namespace exif::details
