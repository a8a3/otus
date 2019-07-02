#include <gtest/gtest.h>
#include "lib.h"

TEST(TestVersion, TestVersionValue)
{
    const int ver = version();
    ASSERT_TRUE( ver > 0 ) << "incorrect version value: " << ver;
}

// ------------------------------------------------------------------
int main( int argc, char* argv[] ) {
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}