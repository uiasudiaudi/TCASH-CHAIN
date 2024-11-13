// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xelect_net/demo/multilayer_network_demo.h"
#include "xpbase/base/tcash_utils.h"

#include <atomic>

using namespace tcash;

namespace tcash {
void SignalCatch(int sig_no) {
    if (SIGTERM == sig_no || SIGINT == sig_no) {
        printf(" click [Enter] to exit.");
        fflush(stdout);
        ::_Exit(0);
        xinfo("node now existing!");
        // do not delete ,just for debug can be quick when terminate
    }
}
}  // namespace tcash
using namespace tcash;

int main(int argc, char ** argv) {
    // register signal
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR || signal(SIGTERM, tcash::SignalCatch) == SIG_ERR || signal(SIGINT, tcash::SignalCatch) == SIG_ERR) {
        xerror("signal failed");
        return 1;
    }

    elect::MulNetDemo demo;
    tcash::base::Config config;
    if (demo.HandleParamsAndConfig(argc, argv, config) != 0) {
        xerror("handle params failed");
        return 1;
    }

    std::string log_path("./log/xtcash.log");
    
    uint16_t delta = 0;
    config.Get("node","delta",delta);
    uint16_t ori_port = 0;
    config.Get("node","local_port",ori_port);
    config.Set("node", "local_port", ori_port + delta);

    config.Get("log", "path", log_path);
    bool log_debug = true;
    config.Get("log", "debug", log_debug);
    bool log_off = false;
    config.Get("log", "off", log_off);
    std::cout << "using log_path:" << log_path << std::endl;
    // demo.InitLog(log_path, log_debug, log_off);

    if (!log_off) {
        xinit_log(log_path.c_str(), true, true);
        if (log_debug) {
            xset_log_level(enum_xlog_level_debug);
        } else {
            xset_log_level(enum_xlog_level_key_info);
        }
    }

    if (!config.Get("node", "node_id", global_node_id)) {
        xerror("no node_id found in config");
        return 1;
    }

    if (!demo.Init(config)) {
        xerror("start elect main init failed!");
        return 1;
    }

    if (!demo.Run(config)) {
        xerror("start elect main failed!");
        return 1;
    }

    std::cout << "begin build xelect_net_demo network..." << std::endl;
    demo.BuildXelectNetDemoNetwork();

    // will block here
    demo.GetElectCmd().Run(delta);

    demo.Stcash();

    xerror("demo main exit");
    return 0;
}
