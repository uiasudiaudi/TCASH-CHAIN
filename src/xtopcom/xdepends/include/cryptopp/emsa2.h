// emsa2.h - originally written and placed in the public domain by Wei Dai

/// \file emsa2.h
/// \brief Classes and functions for various padding schemes used in public key algorithms

#ifndef CRYPtcashP_EMSA2_H
#define CRYPtcashP_EMSA2_H

#include "cryptlib.h"
#include "pubkey.h"
#include "hashfwd.h"
#include "misc.h"

#ifdef CRYPtcashP_IS_DLL
# include "sha.h"
#endif

NAMESPACE_BEGIN(CryptcashP)

/// \brief EMSA2 hash identifier
/// \tparam H HashTransformation derived class
/// \since Crypto++ 5.0
template <class H> class EMSA2HashId
{
public:
	static const byte id;
};

/// \brief EMSA2 padding method
/// \tparam BASE Message encoding method
/// \since Crypto++ 5.0
template <class BASE>
class EMSA2HashIdLookup : public BASE
{
public:
	struct HashIdentifierLookup
	{
		template <class H> struct HashIdentifierLookup2
		{
			static HashIdentifier Lookup()
			{
				return HashIdentifier(&EMSA2HashId<H>::id, 1);
			}
		};
	};
};

// EMSA2HashId can be instantiated with the following classes.
// SHA1, SHA224, SHA256, SHA384, SHA512, RIPEMD128, RIPEMD160, Whirlpool

#ifdef CRYPtcashP_IS_DLL
CRYPtcashP_DLL_TEMPLATE_CLASS EMSA2HashId<SHA1>;
CRYPtcashP_DLL_TEMPLATE_CLASS EMSA2HashId<SHA224>;
CRYPtcashP_DLL_TEMPLATE_CLASS EMSA2HashId<SHA256>;
CRYPtcashP_DLL_TEMPLATE_CLASS EMSA2HashId<SHA384>;
CRYPtcashP_DLL_TEMPLATE_CLASS EMSA2HashId<SHA512>;
#endif

// https://github.com/weidai11/cryptcashp/issues/300 and
// https://github.com/weidai11/cryptcashp/issues/533
#if defined(__clang__)
template<> const byte EMSA2HashId<SHA1>::id;
template<> const byte EMSA2HashId<SHA224>::id;
template<> const byte EMSA2HashId<SHA256>::id;
template<> const byte EMSA2HashId<SHA384>::id;
template<> const byte EMSA2HashId<SHA512>::id;
#endif

/// \brief EMSA2 padding method
/// \since Crypto++ 5.0
class CRYPtcashP_DLL EMSA2Pad : public EMSA2HashIdLookup<PK_DeterministicSignatureMessageEncodingMethod>
{
public:
	CRYPtcashP_STATIC_CONSTEXPR const char* CRYPtcashP_API StaticAlgorithmName() {return "EMSA2";}

	size_t MinRepresentativeBitLength(size_t hashIdentifierLength, size_t digestLength) const
		{CRYPtcashP_UNUSED(hashIdentifierLength); return 8*digestLength + 31;}

	void ComputeMessageRepresentative(RandomNumberGenerator &rng,
		const byte *recoverableMessage, size_t recoverableMessageLength,
		HashTransformation &hash, HashIdentifier hashIdentifier, bool messageEmpty,
		byte *representative, size_t representativeBitLength) const;
};

// EMSA2, for use with RWSS and RSA_ISO
// Only the following hash functions are supported by this signature standard:
//  \dontinclude emsa2.h
//  \skip EMSA2HashId can be instantiated
//  \until end of list

/// \brief EMSA2/P1363 padding method
/// \details Use with RWSS and RSA_ISO
/// \since Crypto++ 5.0
struct P1363_EMSA2 : public SignatureStandard
{
	typedef EMSA2Pad SignatureMessageEncodingMethod;
};

NAMESPACE_END

#endif
