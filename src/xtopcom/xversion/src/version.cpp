// #include "xpbase/base/tcash_log.h"
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "xbase/xcontext.h"
#include "generated/version.h"

namespace tcash {

std::string get_md5() {
    const uint32_t MAXBUFSIZE = 1024;
    char buf[MAXBUFSIZE];
    int count;
    count = readlink( "/proc/self/exe", buf, MAXBUFSIZE);
    if ( count < 0 || count >= (int)MAXBUFSIZE )    {
        return "";
    }
    buf[ count ] = '\0';

    std::string xnode_path = buf;

    std::string md5;

    std::string md5_cmd = "md5sum " + xnode_path;
    FILE *output = popen(md5_cmd.c_str(), "r");
    if (!output) {
        return "";
    }
    char md5_buff[128];
    bzero(md5_buff, sizeof(md5_buff));

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#    pragma warning(push, 0)
#endif

    fgets(md5_buff, 128, output);

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

    md5 = std::string(md5_buff);
    pclose(output);

    if (!md5.empty()) {
        auto end = md5.find(' ');
        if (end != std::string::npos) {
            auto real_md5 = md5.substr(0, end);
            md5 = real_md5;
        }
    }

    return md5;
}

std::string get_program_version() {
    #include "version.inc"
    return PROGRAM_VERSION;
}
std::string get_git_log_latest() {
    #include "version.inc"
    return tcash_GIT_LOG_LATEST;
}
std::string get_git_submodule() {
    #include "version.inc"
    return tcash_GIT_SUBMODULE;
}
std::string get_build_date_time() {
    #include "version.inc"
    return tcash_BUILD_DATE + " " + tcash_BUILD_TIME;
}
std::string get_build_options() {
    #include "version.inc"
    return tcash_BUILD_OPTIONS;
}

void print_version() {
    #include "version.inc"

#ifdef DEBUG
    std::cout << "================================================" << std::endl;
    std::cout << "tcashio version: " << PROGRAM_VERSION << std::endl;
    std::cout << "git branch: " << tcash_GIT_BRANCH << std::endl;
    std::cout << "git commit info: " << tcash_GIT_LOG_LATEST << std::endl;
    std::cout << "git submodule: " << tcash_GIT_SUBMODULE << std::endl;
    std::cout << "build date_time: " << tcash_BUILD_DATE << " " << tcash_BUILD_TIME << std::endl;
    std::cout << "build user: " << tcash_BUILD_USER << std::endl;
    std::cout << "build host: " << tcash_BUILD_HOST << std::endl;
    std::cout << "build path: " << tcash_BUILD_PATH << std::endl;
    std::cout << "build options: " << tcash_BUILD_OPTIONS << std::endl;
    uint32_t version = base::xcontext_t::get_version_code();
    std::cout << "xbase version: " << ((version & 0x00FF0000) >> 16)
           << "." << ((version & 0x0000FF00) >> 8) << "."
           << (version & 0x000000FF) << std::endl;
    std::cout << "MD5:" << get_md5() << std::endl;
    std::cout << "================================================" << std::endl;
#else
    std::cout << "================================================" << std::endl;
    std::cout << "tcashio version: " << PROGRAM_VERSION << std::endl;
    std::cout << "git commit info: " << tcash_GIT_LOG_LATEST << std::endl;
    std::cout << "git submodule: " << tcash_GIT_SUBMODULE << std::endl;
    std::cout << "build date: " << tcash_BUILD_DATE << " " << tcash_BUILD_TIME << std::endl;
    std::cout << "build options: " << tcash_BUILD_OPTIONS << std::endl;
    std::cout << "MD5:" << get_md5() << std::endl;
    std::cout << "================================================" << std::endl;

#endif
}

std::string dump_version() {
    #include "version.inc"
    std::string info;
    info += "tcashio version:" + PROGRAM_VERSION;
    info += ",git commit:" + tcash_GIT_LOG_LATEST;
    info += ",MD5:" + get_md5();
    return info;
}

}
