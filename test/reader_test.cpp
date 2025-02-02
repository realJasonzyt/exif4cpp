#include "reader.h"
#include <cstring>
#include <filesystem>
#include <gtest/gtest.h>

using namespace exif;

char testData[12] =
    {char(0xFF), char(0xD8), char(0xFF), char(0xE1), char(0x14), char(0xC1), 'E', 'x', 'i', 'f', 0x0, 0x0};

class ReaderTest : public ::testing::Test {

protected:
    ReaderTest() : r(file) {}
    static void SetUpTestSuite() {
        std::fstream out("test.bin", std::ios::out | std::ios::binary | std::ios::app);
        out.write(testData, sizeof(testData));
        out.close();
    }
    static void TearDownTestSuite() { std::filesystem::remove("test.bin"); }
    void        SetUp() { file.open("test.bin", std::ios::in | std::ios::binary); }
    void        TearDown() { file.close(); }

    std::fstream    file;
    details::Reader r;
};

TEST_F(ReaderTest, ReadInt) {
    file.seekp(std::ios::beg);
    EXPECT_EQ(r.read<uint8_t>(), 0xFF);
    EXPECT_EQ(r.read<uint8_t>(), 0xD8);
    EXPECT_EQ(r.read<uint16_t>(), 0xFFE1);
    EXPECT_EQ(r.read<uint16_t>(), 0x14C1);
    EXPECT_EQ(r.read<uint32_t>(), 0x45786966);
    uint16_t a;
    r.read(a);
    EXPECT_EQ(a, 0x0);
}

TEST_F(ReaderTest, Read) {
    file.seekp(std::ios::beg);
    EXPECT_EQ(std::memcmp(r.read(sizeof(testData)).data(), testData, sizeof(testData)), 0);
    file.seekp(std::ios::beg);
    EXPECT_EQ(std::memcmp(r.read(2).data(), testData, 2), 0);
}

TEST_F(ReaderTest, ReadUntil) {
    file.seekp(10);
    char tag[2] = {0, 0};
    EXPECT_EQ(std::memcmp(r.readUntil(tag, sizeof(tag)).data(), tag, sizeof(tag)), 0);
    file.seekp(std::ios::beg);
    EXPECT_EQ(std::memcmp(r.readUntil(tag, sizeof(tag)).data(), testData, sizeof(testData)), 0);
    file.seekp(std::ios::beg);
    tag[0] = 0xD8;
    EXPECT_EQ(std::memcmp(r.readUntil(tag, 1).data(), testData, 2), 0);
}

TEST_F(ReaderTest, Skip) {
    file.seekp(std::ios::beg);
    r.skip(2);
    EXPECT_EQ(std::memcmp(r.read(2).data(), testData + 2, 2), 0);
}

TEST_F(ReaderTest, ReverseEndian) {
    file.seekp(std::ios::beg);
    // get platform endian
    union {
        uint32_t i;
        char     c[4];
    } u;
    u.i                = 0x01020304;
    bool is_big_endian = u.c[0] == 1;
    r.setFileEndian(is_big_endian);
    EXPECT_EQ(r.read<uint16_t>(), 0xD8FF);
}