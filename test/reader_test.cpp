#include "reader.h"
#include <cstring>
#include <gtest/gtest.h>

using namespace exif;

TEST(ReaderTest, Read) {
    // init
    std::fstream out("test.bin", std::ios::out | std::ios::binary | std::ios::app);
    char a[12] = {char(0xFF), char(0xD8), char(0xFF), char(0xE1), char(0x14), char(0xC1), 'E', 'x', 'i', 'f', 0x0, 0x0};
    out.write(a, sizeof(a));
    out.close();

    std::fstream    file("test.bin", std::ios::in | std::ios::binary);
    details::reader r(file);
    EXPECT_EQ(r.read<uint8_t>(), 0xFF);
    EXPECT_EQ(r.read<uint8_t>(), 0xD8);
    EXPECT_EQ(r.read<uint16_t>(), 0xFFE1);
    EXPECT_EQ(r.read<uint16_t>(), 0x14C1);
    EXPECT_EQ(r.read<uint32_t>(), 0x45786966);
    char tag[2] = {0, 0};
    EXPECT_EQ(std::memcmp(r.read_until(tag, sizeof(tag)).data(), tag, sizeof(tag)), 0);
    file.seekp(std::ios::beg);
    EXPECT_EQ(std::memcmp(r.read_until(tag, sizeof(tag)).data(), a, sizeof(a)), 0);
    file.seekp(std::ios::beg);
    tag[0] = 0xD8;
    EXPECT_EQ(std::memcmp(r.read_until(tag, 1).data(), a, 2), 0);
    file.close();
}
