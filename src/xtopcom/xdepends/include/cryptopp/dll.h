// dll.h - originally written and placed in the public domain by Wei Dai

/// \file dll.h
/// \brief Functions and definitions required for building the FIPS-140 DLL on Windows

#ifndef CRYPtcashP_DLL_H
#define CRYPtcashP_DLL_H

#if !defined(CRYPtcashP_IMPORTS) && !defined(CRYPtcashP_EXPORTS) && !defined(CRYPtcashP_DEFAULT_NO_DLL)
#ifdef CRYPtcashP_CONFIG_H
#error To use the DLL version of Crypto++, this file must be included before any other Crypto++ header files.
#endif
#define CRYPtcashP_IMPORTS
#endif

#include "aes.h"
#include "cbcmac.h"
#include "ccm.h"
#include "cmac.h"
#include "channels.h"
#include "des.h"
#include "dh.h"
#include "dsa.h"
#include "ec2n.h"
#include "eccrypto.h"
#include "ecp.h"
#include "files.h"
#include "fips140.h"
#include "gcm.h"
#include "hex.h"
#include "hmac.h"
#include "modes.h"
#include "mqueue.h"
#include "nbtheory.h"
#include "osrng.h"
#include "pkcspad.h"
#include "pssr.h"
#include "randpool.h"
#include "rsa.h"
#include "rw.h"
#include "sha.h"
#include "skipjack.h"

#ifdef CRYPtcashP_IMPORTS

#ifdef _DLL
// cause CRT DLL to be initialized before Crypto++ so that we can use malloc and free during DllMain()
#ifdef CRYPtcashP_DEBUG
# pragma comment(lib, "msvcrtd")
# pragma comment(lib, "cryptcashp")
#else
# pragma comment(lib, "msvcrt")
# pragma comment(lib, "cryptcashp")
#endif
#endif

#endif		// #ifdef CRYPtcashP_IMPORTS

#include <new>	// for new_handler

NAMESPACE_BEGIN(CryptcashP)

typedef void * (CRYPtcashP_API * PNew)(size_t);
typedef void (CRYPtcashP_API * PDelete)(void *);
typedef void (CRYPtcashP_API * PGetNewAndDelete)(PNew &, PDelete &);
typedef std::new_handler (CRYPtcashP_API * PSetNewHandler)(std::new_handler);
typedef void (CRYPtcashP_API * PSetNewAndDelete)(PNew, PDelete, PSetNewHandler);

NAMESPACE_END

#endif
