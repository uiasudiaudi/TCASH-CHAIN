// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xchain_fork/xutility.h"

#include "xvledger/xvblock.h"   // for base::tcash_BEGIN_GMTIME

#include <cinttypes>

NS_BEG2(tcash, chain_fork)

bool xtcash_utility::is_forked(tcash::optional<fork_points::xfork_point_t> const & fork_point, uint64_t const target) noexcept {
    if (!fork_point.has_value()) {
        return false;
    }

    xdbg("xtcash_utility::is_forked target:%" PRIu64 ", fork point:%" PRIu64 ", %s", target, fork_point.value().point, fork_point->description.c_str());
    return target >= fork_point.value().point;
}

bool xtcash_utility::is_block_forked(uint64_t const target) noexcept {
    return xtcash_utility::is_forked(fork_points::v11200_block_fork_point, target);
}

NS_END2
