#include <gtest/gtest.h>
#include "test_parse.hh"
#include "test_error.hh"
#include "test_dump.hh"
#include "test_access.hh"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}