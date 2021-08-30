#include "test.h"

TEST(first)
{
    EXPECT(2 == 2);
}

TEST(second)
{
    EXPECT(1 == 2);
}

SUITE(SampleSuite)
{
    RUN(first, second);
}
