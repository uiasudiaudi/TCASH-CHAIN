// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"

#include "xchaininit/xchain_command.h"
#include "xchaininit/xinit.h"
#include "xchaininit/xconfig.h"

#include <sys/time.h>
#if defined(__LINUX_PLATFORM__)
#    include <sys/prctl.h>
#endif
#if defined(XCXX20)
#    include "CLI/CLI.hpp"
#else
#    include "CLI11.hpp"
#endif
#include "xversion/version.h"
#include "tcashio_setproctitle.h"
#include "xbasic/xthreading/xbackend_thread.hpp"
#include "xbasic/xtimer_driver.h"
#include "xbasic/xsys_utl.h"
#include "xcommon/xrole_type.h"
#include "xconfig/xpredefined_configurations.h"
// #include "xmonitor.h"
#include "xsafebox/safebox_http_client.h"
#include "xsafebox/safebox_http_server.h"

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

// nlohmann_json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace tcash;
////////////////////////define configure file path and name/////////////////////////
char ** tcashio_os_argv;
int tcashio_os_argc;
static const std::string tcashIO_HOME_ENV = "tcashIO_HOME";
static const uint32_t MAX_PASSWORD_SIZE = 1024;
static std::atomic<bool> child_reap{false};
static std::atomic<uint32_t> child_exit_code{0};
static pid_t child_pid = -1;
static uint64_t last_restart_timestamp_s{0};
static uint64_t restart_count{0};
static std::string pid_file;
static std::string safebox_pid_file;
static std::vector<std::string> support_cmd_vec = {"help", "version", "node", "mining", "staking", "chain", "transfer", "govern", "resource", "wallet", "db", "debug"};

xnode_signal_t multi_process_signals[] = {{SIGCHLD, "SIGCHLD", "", xnode_signal_handler},
                                          {SIGHUP, "SIGHUP", "", xnode_signal_handler},
                                          {SIGTERM, "SIGTERM", "", xnode_signal_handler},
                                          {SIGINT, "SIGINT", "", xnode_signal_handler},
                                          {0, NULL, "", NULL}};

xnode_signal_t normal_signals[] = {{SIGTERM, "SIGTERM", "", xnode_signal_handler}, {SIGINT, "SIGINT", "", xnode_signal_handler}, {0, NULL, "", NULL}};

static int check_log_file(const std::string & full_log_file_path) {
    // check file's last modified time
    struct stat file_stat;
    if (stat(full_log_file_path.c_str(), &file_stat) != 0) {
        fprintf(stderr, "stat failed, path(%s), detail(%d:%s)", full_log_file_path.c_str(), errno, strerror(errno));
        return -1;
    }

    struct timeval now;
    gettimeofday(&now, nullptr);

    // remove file 6 hours before current time
    int hour = (now.tv_sec - file_stat.st_mtime) / 3600;
    fprintf(stdout, "time diff in hour(%d) for file(%s)", hour, full_log_file_path.c_str());
    if (hour >= 6) {
        fprintf(stdout, "remove log file path(%s)", full_log_file_path.c_str());
        if (remove(full_log_file_path.c_str()) != 0) {
            fprintf(stderr, "remove failed, path(%s), detail(%d:%s)", full_log_file_path.c_str(), errno, strerror(errno));
            return -1;
        }
        return 0;
    }
    return -1;
}

void check_log_path(const std::string & log_path) {
    DIR * dirp = opendir(log_path.c_str());
    if (dirp == nullptr) {
        fprintf(stderr, "opendir failed, path(%s), detail(%d:%s)", log_path.c_str(), errno, strerror(errno));
        return;
    } else {
        struct dirent * entry;
        while ((entry = readdir(dirp)) != nullptr) {
            // skip current directory and parent directory to avoid recurrence.
            if (strncmp(entry->d_name, ".", 1) == 0 || strncmp(entry->d_name, "..", 2) == 0) {
                continue;
            }

            if (DT_REG == entry->d_type || DT_UNKNOWN == entry->d_type) {
                // check log file's last modified time, sample: "xtcash.2019-07-04-144108-1-17497.log"
                if (strncmp(entry->d_name, "xtcash", 4) == 0 && strstr(entry->d_name, ".log") && strlen(entry->d_name) > sizeof("xtcash.log")) {
                    std::string full_file_path = log_path + "/" + entry->d_name;
                    check_log_file(full_file_path);
                }
            }
        }

        closedir(dirp);
    }
}

int load_xtcashchain(config_t & config) {
    if (config.so_func_name == "parse_execute_command") {
        return tcash::parse_execute_command(config.config_file_extra.c_str(), config.argc, config.argv);
    } else if (config.so_func_name == "init_component") {
        tcash::print_version();
        return tcash::tcashchain_init(config.config_file, config.config_file_extra);
    } else if (config.so_func_name == "init_noparams_component") {
        tcash::print_version();
        return tcash::tcashchain_noparams_init(config.pub_key, config.pri_key, config.node_id, config.datadir, config.config_file_extra);
    } else if (config.so_func_name == "decrypt_keystore_by_key") {
        config.pri_key = tcash::decrypt_keystore_by_key(config.keystore_path, config.token);
    } else if (config.so_func_name == "check_miner_info") {
        std::string miner_type;
        bool status = tcash::check_miner_info(config.pub_key, config.node_id, miner_type);
        if (!status) {
            return -1;
        }
        config.token = miner_type;
    } else {
        std::cerr << "unsupport function:" << config.so_func_name << std::endl;
        return -1;
    }
    return 0;
}

// start daemon
bool daemon() {
    int fd;

    switch (fork()) {
    case -1:
        return false;
    case 0:
        // the first child process
        break;
    default:
        // exit if father process
        exit(0);
    }

    // setsid() will create a new session and then, the first child will detach from tty and process group,
    // and become the leader of this session. Right now the first child became the leader of no-tty session,
    // but this child process still has the ability to open  a tty
    if (setsid() == -1) {
        // setsid error
        return false;
    }

    /*
    // the second fork in order to make the first child process non-leader, avoid the ability to open a tty
    switch (fork()) {
        case -1:
            return false;
        case 0:
            // the second process
            break;
        default:
            // exit the first child
            exit(0);
    }
    */

    // reset umask
    umask(0);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        return false;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        return false;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        return false;
    }

#if 0
    if (dup2(fd, STDERR_FILENO) == -1) {
        return;
    }
#endif

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
            return false;
        }
    }

    return true;
}

int spawn_child(config_t & config) {
    pid_t pid = fork();
    switch (pid) {
    case -1: {
        fprintf(stderr, "fork() failed while spawning, details(%d:%s)", errno, strerror(errno));
        return -1;
    }
    case 0: {
        // prctl(PR_SET_NAME, "xnode", NULL, NULL, NULL);
        //  this is child

        const char * new_process_name = "xnode process";
        std::string new_title;
        for (int i = 0; i < tcashio_os_argc && tcashio_os_argv[i]; ++i) {
            if (i == 0) {
                new_title += new_process_name;
                new_title += " ";
            } else {
                new_title += tcashio_os_argv[i];
                new_title += " ";
            }
        }
        if (new_title.back() == ' ') {
            new_title.pop_back();
        }

        // set process title
        if (tcashio_init_setproctitle() == 0) {
            tcashio_setproctitle(new_title.c_str());
        }
        return load_xtcashchain(config);
    }
    default: {
        child_pid = pid;

        const char * new_process_name = "daemon process";
        std::string new_title;
        for (int i = 0; i < tcashio_os_argc && tcashio_os_argv[i]; ++i) {
            if (i == 0) {
                new_title += new_process_name;
                new_title += " ";
            } else {
                new_title += tcashio_os_argv[i];
                new_title += " ";
            }
        }
        if (new_title.back() == ' ') {
            new_title.pop_back();
        }

        // set process title
        if (tcashio_init_setproctitle() == 0) {
            tcashio_setproctitle(new_title.c_str());
        }

        break;
    }
    }  // end switch(pid)

    return 0;
}

int start_worker_monitor_thread(config_t & config) {
    auto thread_proc = std::bind(CheckReStartXtcashchain, config);
    std::thread(thread_proc).detach();

    return 0;
}

int start_monitor_thread(config_t & config) {
#ifndef DEBUG
    // only enable in debug mode
    auto log_thread_proc = std::bind(log_monitor_proc, config);
    std::thread(log_thread_proc).detach();
#endif

    return 0;
}


int log_monitor_proc(config_t config) {
    std::string log_path = config.datadir + "/log";
    int log_scope_sec = 6 * 3600; // max scope 6 hours

    while (true) {
        int available_space_GB = xsys_utl_t::get_log_directory_available_space_GB(log_path);
        if (available_space_GB < 2) {
            log_scope_sec = 10 * 60;  // ten minutes
        } else if (available_space_GB < 5) {
            log_scope_sec = 1 * 3600; // one hour
        }
        int metric_scope_sec = 4 * log_scope_sec;
        xsys_utl_t::check_and_remove_old_log_files(log_path, log_scope_sec, metric_scope_sec);
        std::this_thread::sleep_for(std::chrono::seconds(10 * 60));  // every 10 min
    }
    return 0;
}

void signal_handler(int signo) {
    // avoid malloc dead lock(do not using malloc here, do not use LOG module)
    if (signo == SIGCHLD) {
        get_child_status();
    } else if (signo == SIGHUP) {  // reload, kill + child_reap
#ifdef DEBUG
        std::cout << "catch SIGHUP" << std::endl;
#endif
        if (kill(child_pid, SIGKILL) == -1) {
            std::cout << "kill() failed:" << strerror(errno) << "," << child_pid << "," << getpid() << std::endl;
        }
    } else if (signo == SIGTERM || signo == SIGINT) {
#ifdef DEBUG
        std::cout << "catch SIGTERM or SIGINT, will exit" << std::endl;
#endif
        if (!pid_file.empty()) {
            std::ifstream in_pid(pid_file);
            if (in_pid.is_open()) {
                char buff[16];
                bzero(buff, sizeof(buff));
                in_pid.getline(buff, 16);
                uint32_t xnode_pid = std::stoi(std::string(buff));
                auto runing_xnode_pid = static_cast<uint32_t>(getpid());
                if (xnode_pid == runing_xnode_pid) {
                    remove(pid_file.c_str());
                }
            }
        } else if (!safebox_pid_file.empty()) {
            std::ifstream in_pid(safebox_pid_file);
            if (in_pid.is_open()) {
                char buff[16];
                bzero(buff, sizeof(buff));
                in_pid.getline(buff, 16);
                uint32_t pid = std::stoi(std::string(buff));
                auto runing_safebox_pid = static_cast<uint32_t>(getpid());
                if (pid == runing_safebox_pid) {
                    remove(safebox_pid_file.c_str());
                }
            }
        }

        ::_Exit(0);
    }
}

bool check_process_running(const std::string & pid_file) {
    if (pid_file.empty()) {
        return false;
    }

    std::ifstream in_pid(pid_file);
    if (!in_pid.is_open()) {
        return false;
    }

    char buff[16];
    bzero(buff, sizeof(buff));
    in_pid.getline(buff, 16);
    in_pid.close();

    uint32_t tcashio_pid;
    try {
        tcashio_pid = std::stoi(std::string(buff));
    } catch (...) {
        std::cout << "tcashio.pid error." << std::endl;
        return false;
    }
    // kill 0 test process running or not
    if (kill(tcashio_pid, 0) == -1) {
        // kill 0 error, process not exist
        return false;
    }

    return true;
}

void xnode_signal_handler(int signo, siginfo_t * siginfo, void * ucontext) {
    signal_handler(signo);
}

int register_signals(bool multi_process) {
    xnode_signal_t * sig;
    if (multi_process) {
        sig = multi_process_signals;
    } else {
        sig = normal_signals;
    }
    struct sigaction sa;

    for (; sig->signo != 0; sig++) {
        bzero(&sa, sizeof(struct sigaction));

        if (sig->handler) {
            sa.sa_sigaction = sig->handler;
            sa.sa_flags = SA_SIGINFO;

        } else {
            sa.sa_handler = SIG_IGN;
        }

        sigemptyset(&sa.sa_mask);
        if (sigaction(sig->signo, &sa, NULL) == -1) {
            std::cout << "sigaction(" << sig->signame << ") failed" << std::endl;
        }
    }

    return 0;
}

void get_child_status() {
    int status = 0;
    pid_t pid = -1;
    int err = 0;
    int one = 0;

    for (;;) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid == 0) {
            return;
        }

        if (pid == -1) {
            err = errno;
            if (err == EINTR) {
                continue;
            }
            if (err == ECHILD && one) {
                return;
            }
            if (err == ECHILD) {
                return;
            }
            return;
        }

        // ntpupdate also notify SIGCHLD
        if (pid == child_pid) {
            one = 1;
            child_reap = true;
            std::cout << "child reap" << std::endl;
            // check if killed by sig or by self exit
            if (WTERMSIG(status)) {
                std::cout << "exited with signal:" << WTERMSIG(status) << std::endl;
            } else {
                child_exit_code = WEXITSTATUS(status);
                std::cout << "exited with code:" << WEXITSTATUS(status) << std::endl;
            }
        }

    }  // end for(;;)
}

void CheckReStartXtcashchain(config_t & config) {
    if (!config.multiple_process) {
        return;
    }
    while (true) {
        if (child_reap) {            
            child_reap = false;

            uint64_t now_s = base::xtime_utl::gettimeofday_ms() / 1000;
            // If more than one day, reset to zero 
            if (last_restart_timestamp_s != 0 && (last_restart_timestamp_s + 24*60*60 < now_s) ) {
                last_restart_timestamp_s = 0;
                restart_count = 0;
            }            

            uint64_t max_count = 5;
            if (restart_count < max_count) {
                restart_count++;
            }

            // 30s, 60s, 300s, 3600s, 3*3600s
            uint64_t wait_time_s_array[] = {30, 60, 300, 60*60, 3*60*60};
            int wait_time_s = wait_time_s_array[restart_count - 1];

            std::cout << "worker stcashped, wait to restart... wait_time_s:" << wait_time_s << ",restart_count:" << restart_count << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(wait_time_s));
            // reset last restart time
            last_restart_timestamp_s = base::xtime_utl::gettimeofday_ms() / 1000;

            // load child process auto
            std::cout << "child restart" << std::endl;
            spawn_child(config);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return;
}

// generate extra config or update
bool generate_extra_config(config_t & config) {
    json jextra;
    std::string extra_config = config.datadir + "/.extra_conf.json";
    do {
        if (!isFileExist(extra_config)) {
            break;
        }

        // update config
        std::ifstream in(extra_config);
        if (!in.is_open()) {
            std::cerr << "can't open file: " << extra_config << std::endl;
            break;
        }

        json reader;
        try {
            in >> reader;
        } catch (json::parse_error & e) {
            in.close();
            std::cout << "parse config file:" << extra_config << " failed" << std::endl;
            break;
        }
        jextra = reader;
    } while (0);

    // create config or update config
    jextra["admin_http_addr"] = config.admin_http_addr;
    jextra["admin_http_port"] = config.admin_http_port;
    // other key:values ,such as bootstrap ip
    jextra["net_port"] = config.net_port;
    jextra["bootnodes"] = config.bootnodes;
    jextra["datadir"] = config.datadir;
    jextra["db_compress"] = config.db_compress;

    // write prettified JSON to another file
    std::ofstream out(extra_config);
    out << std::setw(4) << jextra << std::endl;
    out.close();
    config.config_file_extra = extra_config;
    return true;
}

bool get_default_miner(config_t & config, std::map<std::string, std::string> & default_miner) {
    std::string extra_config = config.datadir + "/.extra_conf.json";

    std::ifstream in(extra_config);
    if (!in.is_open()) {
        std::cerr << "can't open file: " << extra_config << std::endl;
        return false;
    }
    json reader;
    try {
        in >> reader;
        default_miner["path"] = reader["default_miner_keystore"].get<std::string>();
        if (!isFileExist(default_miner.at("path"))) {
            std::cout << "can't find default miner keystore file " << reader["default_miner_keystore"].get<std::string>() << std::endl;
            return false;
        }

        default_miner["node_id"] = reader["default_miner_node_id"].get<std::string>();
        default_miner["public_key"] = reader["default_miner_public_key"].get<std::string>();
        // default_miner["private_key"] = reader["default_miner_private_key"].get<std::string>();
    } catch (...) {
        in.close();
        return false;
    }
    in.close();

    // get miner private key from mem(safebox)
    safebox::SafeBoxHttpClient safebox_client{"127.0.0.1:7000"};
    auto prikey = safebox_client.request_prikey(reader["default_miner_public_key"].get<std::string>());
    if (prikey.empty()) {
        return false;
    }
    // for safety, prikey from safebox not final privatekey, it's token
    // default_miner["private_key"] = prikey;
    default_miner["token"] = prikey;

    if (default_miner.at("token").empty()) {
        return false;
    }

    return true;
}

void block_loop() {
    while (true) {
        // sleep forerver
        std::this_thread::sleep_for(std::chrono::seconds(6000));
    }
}

// master-worker mode
void start_master_worker_mode(config_t & config) {
    if (register_signals(true) != 0) {
        std::cerr << "register signals failed" << std::endl;
        return;
    }

    spawn_child(config);
    start_monitor_thread(config);
    start_worker_monitor_thread(config);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    // will block here
    block_loop();
}

// single process mode
void start_single_process_mode(config_t & config) {
    if (register_signals(false) != 0) {
        std::cerr << "register signals failed" << std::endl;
        return;
    }
    auto thread_proc = std::bind(load_xtcashchain, config);
    std::thread(thread_proc).detach();

    start_monitor_thread(config);  // will detach
    // do not need check worker status for single process
    // start_worker_monitor_thread(config);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    // will block here
    block_loop();
}

// rewrite xtcashchain json config file, using base dir datadir
bool update_component_config_file(config_t & config) {
    return true;

#ifdef NDEBUG
    // only for debug mode
    return true;
#endif
    auto config_file = config.config_file;
    if (config_file.empty()) {
        // maybe start with no params
        return true;
    }
    std::ifstream in(config_file);
    if (!in.is_open()) {
        std::cerr << "can't open file: " << config_file << std::endl;
        assert(false);
        return false;
    }
    json reader;
    try {
        in >> reader;
    } catch (json::parse_error & e) {
        in.close();
        std::cout << "parse config file:" << config_file << " failed" << std::endl;
        return false;
    }
    in.close();

    for (auto & el : reader.items()) {
        if (el.key() == "db_path") {
            reader[el.key()] = config.datadir + "/db";
        } else if (el.key() == "log_path") {
            reader[el.key()] = config.datadir + "/log";
            config.com_log_path = config.datadir + "/log";
        }
    }

    reader["datadir"] = config.datadir;

    // write prettified JSON to another file
    std::ofstream out(config.config_file);
    out << std::setw(4) << reader << std::endl;
    out.close();
    return true;
}

bool load_keystore(config_t & config) {
    if (config.datadir.empty()) {
        std::cout << "error:datadir empty" << std::endl;
        return false;
    }

    std::map<std::string, std::string> default_miner;
    if (get_default_miner(config, default_miner)) {
        config.node_id = default_miner.at("node_id");
        config.pub_key = default_miner.at("public_key");
        config.keystore_path = default_miner.at("path");
        // config.pri_key = default_miner.at("private_key");
        // return true;

        // get final private_key from token
        config.so_func_name = "decrypt_keystore_by_key";
        config.token = default_miner.at("token");
        int libret = load_xtcashchain(config);
        if (libret != 0) {
            std::cout << "decrypt keystore:" << config.keystore_path << " failed" << std::endl;
            return false;
        }

        return true;
    }

    std::cout << "Please set a miner key by command 'tcashio mining setMinerKey'" << std::endl;
    return false;
}

// datadir not exist will create it first
bool datadir_check_init(config_t & config) {
    char * tcashio_home = getenv(tcashIO_HOME_ENV.c_str());
    if (!tcashio_home) {
        // if tcashIO_HOME_ENV is not set, using $HOME/tcashnetwork as datadir
        struct passwd * pw = NULL;
        struct group * grp = NULL;

        // get current user info and group info
        do {
            pw = (struct passwd *)getpwuid(getuid());
            if (NULL == pw) {
                // no password entry
                break;
            }

            /*
            printf("login name : %s\n", pw->pw_name);
            printf("user id : %d\n", (int)pw->pw_uid);
            printf("group id : %d\n", (int)pw->pw_gid);
            printf("home dir : %s\n", pw->pw_dir);
            printf("login shell : %s\n", pw->pw_shell);
            */
            config.datadir = pw->pw_dir;
            config.datadir += "/tcashnetwork";

            grp = (struct group *)getgrgid((gid_t)(pw->pw_gid));
            if (NULL == grp) {
                // not group entry
                break;
            }

            // printf("user group name : %s", (const char*)grp->gr_name);
        } while (0);
    } else {
        // tcashIO_HOME_ENV is set, using as datadir
        config.datadir = tcashio_home;
    }

    if (config.datadir.back() == '/') {
        config.datadir.pop_back();
    }

    std::string log_path = config.datadir + "/log";
    config.com_log_path = log_path;
    config.pid_file = config.datadir + "/tcashio.pid";
    config.safebox_pid_file = config.datadir + "/safebox.pid";

    if (!isDirExist(config.datadir)) {
        int mk_status = mkdir(config.datadir.c_str(), 0755);
        if (mk_status != 0) {
            if (errno == EACCES) {
                std::cout << "mkdir path:" << config.datadir << " failed, Permission denied" << std::endl;
            } else {
                std::cout << "mkdir path:" << config.datadir << " failed" << std::endl;
            }
            return false;
        }
    } else {
        // if dir exist, judge write access
        int a_status = access(config.datadir.c_str(), W_OK);
        if (a_status != 0) {
            std::cout << "Permission denied to access path:" << config.datadir << std::endl;
            return false;
        }
    }

    // do not need care about return code, because it's not important
    mkdir(config.com_log_path.c_str(), 0755);
    return true;
}

// get current dir
std::string get_working_path() {
    char temp[256];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

// start http server for password cache.
int StartNodeSafeBox(const std::string & safebox_addr, uint16_t safebox_port, std::string const & pid_file) {
    // register signal
    if (register_signals(false) != 0) {
        std::cerr << "StartNodeSafeBox register signals failed" << std::endl;
        return -1;
    }

    if (!daemon()) {
        std::cout << "create daemon process failed" << std::endl;
        return -1;
    }

    // set pid
    auto pid = getpid();
    std::ofstream out_pid(pid_file);

    if (!out_pid.is_open()) {
        std::cout << "dump xnode pid:" << pid << " to file:" << pid_file << " failed\n";
        return -1;
    }

    out_pid << std::to_string(pid);
    out_pid.flush();
    out_pid.close();
    safebox_pid_file = pid_file;

    // set process title
    std::string new_title = "node safebox process";
    if (tcashio_init_setproctitle() == 0) {
        tcashio_setproctitle(new_title.c_str());
    }

    // now is daemon

    auto io_obj = std::make_shared<tcash::xbase_io_context_wrapper_t>();
    auto timer_driver = std::make_shared<tcash::xbase_timer_driver_t>(io_obj);
    auto safebox_http_server = std::make_shared<safebox::SafeBoxHttpServer>(safebox_addr, safebox_port, timer_driver);
    if (!safebox_http_server) {
        return -1;
    }
    timer_driver->start();

    try {
        // will block here
        safebox_http_server->Start();
    } catch (...) {
        std::cout << "start safebox http error" << std::endl;
        return -1;
    }
    return 0;
}

int ReloadNode(config_t & config) {
    std::ifstream in_pid(config.pid_file);
    if (!in_pid.is_open()) {
        std::cout << "open xnode pid_file:" << config.pid_file << " failed" << std::endl;
        return false;
    }

    char buff[16];
    bzero(buff, sizeof(buff));
    in_pid.getline(buff, 16);
    uint32_t xnode_pid = std::stoi(std::string(buff));
    in_pid.close();
    std::cout << "found runing xnode_pid:" << xnode_pid << std::endl;

    std::cout << "will send SIGHUP signal to pid:" << xnode_pid << std::endl;
    if (kill(xnode_pid, SIGHUP) == -1) {
        std::cout << "kill(SIGHUP) failed:" << strerror(errno) << "," << xnode_pid << "," << getpid() << std::endl;
        return false;
    }
    std::cout << "tcashio:" << xnode_pid << " will reload after seconds" << std::endl;
    return true;
}

int StcashNode(config_t & config) {
    std::ifstream in_pid(config.pid_file);
    if (!in_pid.is_open()) {
        std::cout << "open xnode pid_file:" << config.pid_file << " failed" << std::endl;
        return false;
    }

    char buff[16];
    bzero(buff, sizeof(buff));
    in_pid.getline(buff, 16);
    uint32_t xnode_pid = std::stoi(std::string(buff));
    in_pid.close();
    std::cout << "found runing tcashio pid:" << xnode_pid << std::endl;

    // std::cout << "will send SIGKILL signal to pid:" << xnode_pid << std::endl;
    //  will exit xnode process
    if (kill(xnode_pid, SIGHUP) == -1) {
        std::cout << "kill(SIGHUP) failed:" << strerror(errno) << "," << xnode_pid << "," << getpid() << std::endl;
        return false;
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    // will exit tcashio process
    if (kill(xnode_pid, SIGTERM) == -1) {
        std::cout << "kill(SIGKILL) failed:" << strerror(errno) << "," << xnode_pid << "," << getpid() << std::endl;
        return false;
    }
    std::cout << "Stcash the node successfully." << std::endl;
    return true;
}

// old way, xtcashchain mode
int StartNodeWithConfig(config_t & config) {
    if (config.config_file.empty()) {
        std::cout << "empty config_file for tcashio(xtcashchain mode)" << std::endl;
        return -1;
    }
    std::cout << "======================================================start tcashio in xtcashchain mode==================================================================="
              << std::endl;
    config.so_func_name = "init_component";
    return load_xtcashchain(config);
}

int StartNode(config_t & config) {
    if (config.config_file.empty()) {  // load from keystore
        if (check_process_running(config.pid_file)) {
            std::cout << "tcashio already running, Aborting!" << std::endl;
            return -1;
        }
        if (!load_keystore(config)) {
            return -1;
        }

        if (!config.genesis) {
            // check miner info
            config.so_func_name = "check_miner_info";
            int miner_status = load_xtcashchain(config);
            if (miner_status != 0) {
                // check miner info not ok: not registered or pub key not matched
                std::cout << "Start node failed." << std::endl;
                return false;
            }
#if !defined(XBUILD_DEV) && !defined(XBUILD_CI) && !defined(XBUILD_GALILEO) && !defined(XBUILD_BOUNTY)
            if (config.token == common::XMINER_TYPE_ADVANCE) {
                std::string dbdir = config.datadir + DB_PATH;
                std::cout << "checking database directory: " << dbdir << std::endl;
                if (IsDirEmpty(dbdir.c_str())) {
                    std::cout << "Please download database. Node type: " << config.token << std::endl;
                    std::cout << "Start node failed." << std::endl;
                    return false;
                }
            }
#endif
        }
    } else {  // read from config
        auto & tcashio_config = tcash::xtcashio_config_t::get_instance();
        tcashio_config.load_config_file(config.config_file);
        config.node_id = tcashio_config.get_string("node_id");
        config.pub_key = tcashio_config.get_string("public_key");
        std::cout << "node_id: " << config.node_id << "\n";
        std::cout << "public_key: " << config.pub_key << "\n";
    }

#ifdef DEBUG
    std::cout << "datadir:       " << config.datadir << std::endl << "node_id:       " << config.node_id << std::endl << "public_key:    " << config.pub_key << std::endl;
#endif

    generate_extra_config(config);

    // judge if using config file
    if (!config.config_file.empty()) {
        // using config
        int res = StartNodeWithConfig(config);
        if (res != 0) {
            std::cout << "Start node failed!\n";
            return res;
        }

        std::cout << "Start node successfully.\n";
        return 0;
    }

    if (config.daemon) {
        std::cout << "Start node successfully." << std::endl;
        // start as daemon process
        if (!daemon()) {
            std::cout << "create daemon process failed" << std::endl;
            return -1;
        } else {
            std::cout << "daemon process started" << std::endl;
        }
    }

    uint32_t xnode_pid = getpid();
    std::ofstream out_pid(config.pid_file);
    if (!out_pid.is_open()) {
        std::cout << "dump xnode pid:" << xnode_pid << " to file:" << config.pid_file << " failed" << std::endl;
        return -1;
    }

    out_pid << std::to_string(xnode_pid);
    out_pid.flush();
    out_pid.close();
    pid_file = config.pid_file;

    config.so_func_name = "init_noparams_component";
    if (!config.multiple_process) {
        start_single_process_mode(config);
    } else {
        start_master_worker_mode(config);
    }
    return 0;
}

int ExecuteCommands(int argc, char * argv[], config_t & config) {
    config.so_func_name = "parse_execute_command";
    /*
    config.params_line = "";
    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) == 0) {
            continue;
        }

        config.params_line += argv[i];
        config.params_line += " ";
    }
    */
    config.argc = argc;
    config.argv = argv;

    return load_xtcashchain(config);
}

int filter_node_commandline(config_t & config, int argc, char * argv[]) {
    if (argc == 1) {
        return 1;
    }
    if (argc >= 3) {
        std::string tmp1(argv[1]);
        std::transform(tmp1.begin(), tmp1.end(), tmp1.begin(), ::tolower);
        std::string tmp2(argv[2]);
        std::transform(tmp2.begin(), tmp2.end(), tmp2.begin(), ::tolower);
        if (tmp1 == "db" && (tmp2 == "prune" || tmp2 == "compact" || tmp2 == "convert")) {
            if (check_process_running(config.pid_file) == true) {
                std::cout << "Please stcash node." << std::endl;
                return 0;
            }
        }
    }
    for (auto i = 0; i < argc; ++i) {
        std::string tmp(argv[i]);
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        if (tmp == "-h" || tmp == "--help") {
            // filter "node [subcommand] -h"
            return 1;
        }
        if (i == 1 && tmp != "node") {
            return 1;
        }
        if (i == 2) {
            if (tmp != "startnode" && tmp != "stcashnode" && tmp != "reloadnode" && tmp != "safebox") {
                return 1;
            }
        }
    }

    // only focus on "tcashio node [startNode/stcashNode/reloadNode] [options not help]

    CLI::App app{"tcashio - the cpp-tcashnetwork command line interface"};
    app.ignore_case();
    // require at least one command
    app.require_subcommand(1);

    // node
    auto node = app.add_subcommand("node", "node command");
    node->require_subcommand(1);
    // startnode
    auto startnode = node->add_subcommand("startNode", "start tcashio");
    int single_process_flag;
    startnode->add_flag("-S,--single_process", single_process_flag, "start tcashio as single_process(default master+worker mode)");
    // startnode->add_option("--cpu_net_interval", config.cpu_net_interval, "the interval of monitoring cpu and net (default: 10)");
    startnode->add_option("--admin_http_addr", config.admin_http_addr, "admin http server addr(default: 127.0.0.1)");
    startnode->add_option("--admin_http_port", config.admin_http_port, "admin http server port(default: 8000)");
    startnode->add_option("--bootnodes", config.bootnodes, "Comma separated endpoints(ip:port) for P2P  discovery bootstrap");
    startnode->add_option("--net_port", config.net_port, "p2p network listening port (default: 9000)");
    startnode->add_option("-c,--config", config.config_file, "start with config file");
    startnode->add_option("--db_compress", config.db_compress, "set db compress option:(default:default_compress, high_compress, bottom_compress, no_compress).");

    int32_t genesis_flag;
    startnode->add_flag("-g,--genesis", genesis_flag, "start with genesis mode");
    int32_t nodaemon_flag;
    startnode->add_flag("--nodaemon", nodaemon_flag, "start as no daemon");
    startnode->callback([&]() -> int {
        if (single_process_flag == 1) {
            config.multiple_process = false;
        }
        if (nodaemon_flag == 1) {
            config.daemon = false;
        }
        if (genesis_flag == 1) {
            config.genesis = true;
        }

        return StartNode(config);
    });

    // stcashnode
    auto stcashnode = node->add_subcommand("stcashNode", "stcash tcashio");
    stcashnode->callback([&]() -> int { return StcashNode(config); });

    // reloadnode
    auto reloadnode = node->add_subcommand("reloadNode", "reload tcashio");
    reloadnode->callback([&]() -> int { return ReloadNode(config); });

    auto safeboxnode = node->add_subcommand("safebox", "safebox process, manager accounts and token");
    std::string safebox_addr = safebox::safebox_default_addr;
    uint16_t safebox_port = safebox::safebox_default_port;
    safeboxnode->add_option("--http_addr", safebox_addr, "safebox http server addr(default: 127.0.0.1)");
    safeboxnode->add_option("--http_port", safebox_port, "safebox http server port(default: 7000)");
    safeboxnode->callback([&]() -> int {
        if (check_process_running(config.safebox_pid_file)) {
            std::cout << "safebox already running" << std::endl;
            return -1;
        }
        return StartNodeSafeBox(safebox_addr, safebox_port, config.safebox_pid_file);
    });

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError & e) {
        app.exit(e);
        return -1;
    }

    return 0;
}

int main(int argc, char * argv[]) {
    tcashio_os_argv = argv;
    tcashio_os_argc = argc;
    config_t config;

    if (!datadir_check_init(config)) {
        std::cout << config.datadir << " invalid, please check" << std::endl;
        return -1;
    }

    if (auto r = filter_node_commandline(config, argc, argv) != 1) {
        return r;
    }

    generate_extra_config(config);

    // handle other(tcashcl/xnode/db) commands
    auto estatus = ExecuteCommands(argc, argv, config);

    if (!check_process_running(config.safebox_pid_file)) {
        // every command will try to load safebox http server
        StartNodeSafeBox(safebox::safebox_default_addr, safebox::safebox_default_port, config.safebox_pid_file);
    }

    return estatus;
}  // end main
