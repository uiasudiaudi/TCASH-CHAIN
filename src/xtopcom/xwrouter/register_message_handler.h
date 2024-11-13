// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xcxx_config.h"
#include "xbase/xpacket.h"
#if defined(XCXX20)
#include "xkad/proto/ubuntu/kadmlia.pb.h"
#else
#include "xkad/proto/centos/kadmlia.pb.h"
#endif
#include "xtransport/transport_fwd.h"

#include <functional>

namespace tcash {
namespace transport {
typedef std::function<void(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet)> HandlerProc;
}
namespace wrouter {

void WrouterRegisterMessageHandler(int msg_type, transport::HandlerProc handler_proc);
void WrouterUnregisterMessageHandler(int msg_type);

}  // namespace wrouter

}  // namespace tcash
