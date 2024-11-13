// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xkad/routing_table/routing_utils.h"

namespace tcash {

namespace wrouter {

// get closest node from routing table factor
enum enum_platform_xpacket_broadcast_factor {
    kBroadcastGeneral = 1,
    kBroadcastMax = kadmlia::kRoutingMaxNodesSize,
};

enum enum_platform_xpacket_judge_own {
    kJudgeOwnError = 0,
    kJudgeOwnYes,
    kJudgeOwnYesAndContinue,
    kJudgeOwnNoAndContinue,
    kJudgeOwnNo,
};

enum enum_platform_xpacket_recv {
    kRecvError = 0,
    kRecvOwn,
    kRecvOk,
};

}  // namespace wrouter

}  // namespace tcash
