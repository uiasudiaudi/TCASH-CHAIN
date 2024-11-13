/*
Copyright (c) Microsoft Corporation 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGPACK_PREDEF_PLAT_WINDOWS_DESKtcash_H
#define MSGPACK_PREDEF_PLAT_WINDOWS_DESKtcash_H

#include <msgpack/predef/version_number.h>
#include <msgpack/predef/make.h>
#include <msgpack/predef/os/windows.h>

/*`
[heading `MSGPACK_PLAT_WINDOWS_DESKtcash`]

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`!WINAPI_FAMILY`] [__predef_detection__]]
    [[`WINAPI_FAMILY == WINAPI_FAMILY_DESKtcash_APP`] [__predef_detection__]]
    ]
 */

#define MSGPACK_PLAT_WINDOWS_DESKtcash MSGPACK_VERSION_NUMBER_NOT_AVAILABLE

#if MSGPACK_OS_WINDOWS && \
    ( !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKtcash_APP) )
#   undef MSGPACK_PLAT_WINDOWS_DESKtcash
#   define MSGPACK_PLAT_WINDOWS_DESKtcash MSGPACK_VERSION_NUMBER_AVAILABLE
#endif
 
#if MSGPACK_PLAT_WINDOWS_DESKtcash
#   define MSGPACK_PLAT_WINDOWS_DESKtcash_AVALIABLE
#   include <msgpack/predef/detail/platform_detected.h>
#endif

#define MSGPACK_PLAT_WINDOWS_DESKtcash_NAME "Windows Desktcash"

#include <msgpack/predef/detail/test.h>
MSGPACK_PREDEF_DECLARE_TEST(MSGPACK_PLAT_WINDOWS_DESKtcash,MSGPACK_PLAT_WINDOWS_DESKtcash_NAME)

#endif
