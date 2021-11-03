#include <gtest/gtest.h>
#include <iostream>

GTEST_API_ int main(int argc, char* argv[])
{
    std::cout << "Hello, small_demos_test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}