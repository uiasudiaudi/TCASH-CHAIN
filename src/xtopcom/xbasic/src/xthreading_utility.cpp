#include "xbasic/xthreading/xutility.h"

NS_BEG2(tcash, threading)

template
struct xtcash_lock_guard_helper<std::mutex>;

NS_END2
