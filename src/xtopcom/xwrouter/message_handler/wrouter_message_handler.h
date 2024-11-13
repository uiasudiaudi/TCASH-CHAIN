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
#include "xwrouter/register_message_handler.h"

#include <array>
#include <map>
#include <mutex>

namespace tcash {

namespace kadmlia {
class RoutingTable;
class ThreadHandler;
}  // namespace kadmlia

namespace wrouter {
class Wrouter;

static const int32_t MsgHandlerMaxSize = 4096;
using ArrayHandlers = std::array<transport::HandlerProc, MsgHandlerMaxSize>;
using MapRequestType = std::map<int, int>;

class WrouterMessageHandler {
private:
    friend class Wrouter;
    friend class ThreadHandler;
    friend void WrouterRegisterMessageHandler(int msg_type, transport::HandlerProc handler_proc);
    friend void WrouterUnregisterMessageHandler(int msg_type);

    static WrouterMessageHandler * Instance();
    void HandleMessage(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);

    void HandleSyncMessage(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet);
    void AddHandler(int msg_type, transport::HandlerProc handler_proc);

    void RemoveHandler(int msg_type);

    WrouterMessageHandler();
    ~WrouterMessageHandler();

    ArrayHandlers array_handlers_;
    std::mutex map_request_type_mutex_;
    std::map<int, int> map_request_type_;
};

typedef std::shared_ptr<WrouterMessageHandler> MessageHandlerPtr;

}  // namespace wrouter

}  // namespace tcash
