#include <BSTest.h>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

bool pretest()
{
    return true;
}

bool compareBuffers(uint32_t *first, uint32_t *second, size_t offset, size_t len)
{
    uint8_t *firstBytes = (uint8_t *)first;
    uint8_t *secondBytes = (uint8_t *)second;

    for (size_t i = offset; i < offset + len; i++)
    {
        if (firstBytes[i] != secondBytes[i])
        {
            Serial.printf("Compare fail @ %u\n", i);
            for (size_t j = i & ~3; j < (i & ~3) + 4; j++)
            {
                Serial.printf("%02x ", firstBytes[j]);
            }
            Serial.println();
            for (size_t j = i & ~3; j < (i & ~3) + 4; j++)
            {
                Serial.printf("%02x ", secondBytes[j]);
            }
            Serial.println();
            return false;
        }
    }
    return true;
}

bool testFlash(uint32_t start_offset, uint8_t data_offset, size_t amount)
{
    static uint32_t *write_buffer = (uint32_t *)malloc(4096);
    static uint32_t *read_buffer = (uint32_t *)malloc(4096);

    for (uint32_t i = 0; i < 1024; i++)
    {
        write_buffer[i] = (i + 100) * 33;
        read_buffer[i] = 0xAAAAAAAA;
    }
    Serial.println("---------------------------------------------------");
    ESP.flashEraseSector(start_offset / 0x1000);
    Serial.printf("Testing %d bytes @ %08x + %d\n", amount, start_offset, data_offset);
    unsigned long start = micros();

    if (!ESP.flashWrite(start_offset, (uint8_t *)write_buffer + data_offset, amount))
    {
        Serial.printf("Write fail\n");
        return false;
    }
    if (!ESP.flashRead(start_offset, (uint8_t *)read_buffer + data_offset, amount))
    {
        Serial.printf("Read fail\n");
        return false;
    }
    if (!compareBuffers(write_buffer, read_buffer, data_offset, amount))
    {
        return false;
    }
    Serial.printf("Write took %lu us\n", micros() - start);
    return true;
}

//        Columns in test case names are as following:
//        1. Offset -> +o (4 byte aligned), -o (unaligned)
//        2. Memory pointer -> +m (4 byte aligned), -m (unaligned)
//        3. Size -> +s (4 byte ), -s (unaligned)
//        4. Number of pages crossed -> np

// Aligned offset
//   Aligned memory
//     Aligned size
TEST_CASE("|+o|+m|+s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 0, 100));
}
TEST_CASE("|+o|+m|+s|1p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 0, 512));
}
//     Unaligned size
TEST_CASE("|+o|+m|-s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 0, 101));
}
TEST_CASE("|+o|+m|-s|2p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 0, 515));
}
//   Unaligned memory
//     Aligned size
TEST_CASE("|+o|-m|+s|0|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 1, 100));
}
TEST_CASE("|+o|-m|+s|1p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 3, 512));
}
//     Unaligned size
TEST_CASE("|+o|-m|-s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 2, 101));
}
TEST_CASE("|+o|-m|-s|2p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0000, 1, 515));
}
// Unaligned offset
//   Aligned memory
//     Aligned size
TEST_CASE("|-o|+m|+s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 0, 100));
}
TEST_CASE("|-o|+m|+s|1p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 0, 260));
}
//     Unaligned size
TEST_CASE("|-o|+m|-s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 0, 105));
}
TEST_CASE("|-o|+m|-s|1p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 0, 271));
}
//   Unaligned memory
//     Aligned size
TEST_CASE("|-o|-m|+s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 1, 100));
}
TEST_CASE("|-o|-m|+s|1p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 2, 260));
}
//     Unaligned size
TEST_CASE("|-o|-m|-s|0p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 3, 105));
}
TEST_CASE("|-o|-m|-s|1p|", "[spi_flash]")
{
    CHECK(testFlash(0xa0001, 1, 271));
}

TEST_CASE("Last bytes of page", "[spi_flash]")
{
    CHECK(testFlash(0xa0000 + 255, 0, 1));
    CHECK(testFlash(0xa0000 + 255, 1, 1));
    CHECK(testFlash(0xa0000 + 254, 0, 2));
    CHECK(testFlash(0xa0000 + 254, 1, 2));
    CHECK(testFlash(0xa0000 + 253, 0, 3));
    CHECK(testFlash(0xa0000 + 253, 1, 3));
}

TEST_CASE("Unaligned page cross only", "[spi_flash]")
{
    CHECK(testFlash(0xa0000 + 254, 0, 3));
    CHECK(testFlash(0xa0000 + 254, 1, 3));
    CHECK(testFlash(0xa0000 + 255, 0, 2));
    CHECK(testFlash(0xa0000 + 255, 1, 2));
}

TEST_CASE("Unaligned page cross with unaligned size (#8588)", "[spi_flash]")
{
    CHECK(testFlash(0xa00b, 0, 202));
}

void loop ()
{
}
