# xwrouter
A High Performance intelligent routing module.

# Feature highlights
+ Fully designed for Multi-Cores and Muti-Threads
+ High Performance & Low memory/cpu consumption
+ intelligent routing for point-to-point communication and broadcast communication
+ multilayered network implementation
+ Cross-Network Communication Techniques
+ iOS, Android, Windows, MacOS, Linux support


# Example

## C++ example
the demo program can be found in demo or tests directory.

```
int main(int argc, char** argv) {
#ifdef _WIN32
    // register signal
    if (signal(SIGTERM, tcash::SignalCatch) == SIG_ERR ||
            signal(SIGINT, tcash::SignalCatch) == SIG_ERR) {
        tcash_FATAL("signal failed");
        return 1;
    }
#else
    // register signal
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR ||
            signal(SIGTERM, tcash::SignalCatch) == SIG_ERR ||
            signal(SIGINT, tcash::SignalCatch) == SIG_ERR) {
        tcash_FATAL("signal failed");
        return 1;
    }
#endif
    tcash::base::Config config;
    if (tcash::HandleParamsAndConfig(argc, argv, config) != 0) {
        tcash_FATAL("handle params and config failed!");
        return 1;
    }

    if (tcash::InitDb(config) != 0) {
        tcash_FATAL("init db failed!");
        return 1;
    }

    if (!tcash::kadmlia::CreateGlobalXid(config)) {
        tcash_FATAL("create global xid failed");
        return 1;
    }
    tcash::kadmlia::CallbackManager::Instance();
    tcash::wrouter::SmallNetNodes::Instance();
    tcash::wrouter::SmallNetNodes::Instance()->Init();

#ifdef USE_REDIS
    std::string redis_ip;
    config.Get("redis", "ip", redis_ip);
    uint16_t redis_port;
    config.Get("redis", "port", redis_port);
    std::cout << "now connect redis: " << redis_ip << ":" << redis_port << std::endl;
    try {
        if (!tcash::base::RedisClient::Instance()->Start(redis_ip, redis_port)) {
            std::cout << "start redis failed!" << std::endl;
            return 1;
        }
    } catch (...) {}
#endif

    int res = tcash::MainEdge(config);
    tcash_FATAL("main exit: %d", res);
    return res;
}
```


# Contact
[tcash Network](https://www.tcashnetwork.org/)

# License
Copyright (c) 2017-2019 Telos Foundation & contributors

Distributed under the MIT software license, see the accompanying

file COPYING or http://www.opensource.org/licenses/mit-license.php.