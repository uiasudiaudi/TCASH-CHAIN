#include <gtest/gtest.h>
#include <memory>
#include "xbase/xlog.h"

int main(int argc, char * argv[]) {
    testing::InitGoogleTest(&argc, argv);
    xinit_log("./test_mbus_event.log", true, true);
    xset_log_level(enum_xlog_level_debug);
    return RUN_ALL_TESTS();
}
