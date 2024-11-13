//
//  tcash_log.h
//
//  Created by @author on 01/15/2019.
//  Copyright (c) 2017-2019 Telos Foundation & contributors
//

#pragma once

#include <string.h>
#include <iostream>

#include "xbase/xbase.h"

#ifdef _WIN32
#define tcash_LOG_FILE_NAME strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
#else
#define tcash_LOG_FILE_NAME strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#endif

#undef __MODULE__
#define __MODULE__  "xnetwork"

#ifdef DEBUG
    #define tcash_DEBUG(...)          xwrite_log2(__MODULE__,tcash_LOG_FILE_NAME,__FUNCTION__,__LINE__,enum_xlog_level_debug,__VA_ARGS__) 
    #define tcash_INFO(...)           xwrite_log2(__MODULE__,tcash_LOG_FILE_NAME,__FUNCTION__,__LINE__,enum_xlog_level_info,__VA_ARGS__)
    #define tcash_KINFO(...)          xwrite_log2(__MODULE__,tcash_LOG_FILE_NAME,__FUNCTION__,__LINE__,enum_xlog_level_key_info,__VA_ARGS__)
    #define tcash_WARN(...)           xwrite_log2(__MODULE__,tcash_LOG_FILE_NAME,__FUNCTION__,__LINE__,enum_xlog_level_warn,__VA_ARGS__)
    #define tcash_WARN2(...)           xwrite_log2(__MODULE__,tcash_LOG_FILE_NAME,__FUNCTION__,__LINE__,enum_xlog_level_warn,__VA_ARGS__)
    #define tcash_ERROR(...)          xwrite_log2(__MODULE__,tcash_LOG_FILE_NAME,__FUNCTION__,__LINE__,enum_xlog_level_warn,__VA_ARGS__)
#else //release
    #define tcash_DEBUG(...)          void(0)
    #define tcash_INFO(...)           xwrite_log(__MODULE__,enum_xlog_level_info,__VA_ARGS__)
    #define tcash_KINFO(...)          xwrite_log(__MODULE__,enum_xlog_level_key_info,__VA_ARGS__)
    #define tcash_WARN(...)           xwrite_log(__MODULE__,enum_xlog_level_warn,__VA_ARGS__)
    #define tcash_WARN2(...)           xwrite_log(__MODULE__,enum_xlog_level_warn,__VA_ARGS__)
    #define tcash_ERROR(...)          xwrite_log(__MODULE__,enum_xlog_level_warn,__VA_ARGS__)
#endif

#ifdef DEBUG
    #define tcash_DBG_INFO tcash_INFO
#else
    #define tcash_DBG_INFO tcash_DEBUG
#endif

// for logging both in file and console
#define tcash_FATAL(fmt, ...) do { \
    xwrite_log2(__MODULE__, tcash_LOG_FILE_NAME, __FUNCTION__,__LINE__, enum_xlog_level_key_info, fmt, ## __VA_ARGS__); \
    char buf[1024]; \
    snprintf(buf, 1024, fmt, ## __VA_ARGS__); \
    char buf_pre[1024]; \
    snprintf(buf_pre, 1024, "[%s:%d:%s]", tcash_LOG_FILE_NAME, __LINE__, __FUNCTION__); \
    std::cout << buf_pre << " " << buf << std::endl; \
} while (0)
