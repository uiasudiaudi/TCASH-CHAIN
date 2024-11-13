// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xpbase/base/tcash_utils.h"

#include <dlfcn.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <map>
#include <string>

struct config_t {
    bool multiple_process{true};
    std::string target_component_name{"xtcashchain"};
    std::string config_file;
    std::string config_file_extra;
    std::string output_file;
    std::string xnode_path;
    std::string xnode_dirname;
    uint32_t db_backup_id;
    std::string pub_key;
    std::string pri_key;
    std::string token;  // generated from password
    std::string datadir;
    std::string keystore_path;
    std::string node_id;
    uint16_t cpu_net_interval{10};  // monitor cpu and net, default 10s
    std::string pid_file;
    std::string safebox_pid_file;
    std::string com_log_path;
    std::string db_target_path;
    std::string admin_http_addr{"127.0.0.1"};  // admin http server
    uint16_t admin_http_port{8000};            // admin http server port
    uint16_t net_port{9000};                   // network listening port
    std::string bootnodes;                     // Comma separated endpoints(ip:port) for P2P  discovery bootstrap
    std::string so_func_name;                  // func name in libxtcashchain.so
    bool daemon{true};                         // start as daemon process default
    bool genesis{false};
    std::string db_compress;
    int argc;
    char ** argv;
};

typedef struct {
    int signo;
    const char * signame;
    const char * name;
    void (*handler)(int signo, siginfo_t * siginfo, void * ucontext);
} xnode_signal_t;

bool check_process_running(const std::string & pid_file);
void xnode_signal_handler(int signo, siginfo_t * siginfo, void * ucontext);
int load_xtcashchain(config_t & config);
int load_lib(config_t & config);
int start_monitor_thread(config_t & config);
int log_monitor_proc(config_t config);  // ntp and log monitor
int cpu_monitor_proc(config_t config);  // cpu/mem/net monitor
bool daemon();
int spawn_child(config_t & config);
void block_loop();
void CheckReStartXtcashchain(config_t & config);
int start_worker_monitor_thread(config_t & config);
// datadir valid check and init subdir log
bool datadir_check_init(config_t & config);

bool load_keystore(config_t & config);
bool generate_extra_config(config_t & config);

int register_signals(bool multi_process);
void get_child_status();
void start_single_process_mode(config_t & config);
void start_master_worker_mode(config_t & config);

std::string get_working_path();

bool get_default_miner(config_t & config, std::map<std::string, std::string> & default_miner);

int StartNodeWithConfig(config_t & config);
int StartNode(config_t & config);
int StcashNode(config_t & config);
int ReloadNode(config_t & config);
int StartNodeSafeBox(const std::string & safebox_addr, uint16_t safebox_port, std::string const & pid_file);
