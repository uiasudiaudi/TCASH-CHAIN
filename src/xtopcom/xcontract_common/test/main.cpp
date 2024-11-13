#include <gtest/gtest.h>
#include "xbase/xlog.h"

using namespace tcash;

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    xinit_log("./xproperty_test.log", true, true);
    xset_log_level(enum_xlog_level_debug);
    return RUN_ALL_TESTS();
}