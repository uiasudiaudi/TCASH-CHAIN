#include "xtcashcl/include/global_definition.h"
#include "xtcashcl/include/tcashcl.h"
#include "xtcashcl/include/user_info.h"

int main(int argc, const char ** argv) {
    tcash::xtcashcl::xtcashcl xtcash_cl;
    xtcash_cl.api.change_trans_mode(true);
    return 0;
}
