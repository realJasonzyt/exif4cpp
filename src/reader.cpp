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

reader::reader(std::fstream& ifs) : stream_(ifs), is_big_endian_(isBigEndian()) {}

std::string reader::read_until(char* tag, size_t size) {
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

} // namespace exif::details
