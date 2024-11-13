// argnames.h - originally written and placed in the public domain by Wei Dai

/// \file argnames.h
/// \brief Standard names for retrieving values by name when working with \p NameValuePairs

#ifndef CRYPtcashP_ARGNAMES_H
#define CRYPtcashP_ARGNAMES_H

#include "cryptlib.h"

NAMESPACE_BEGIN(CryptcashP)

DOCUMENTED_NAMESPACE_BEGIN(Name)

#define CRYPtcashP_DEFINE_NAME_STRING(name)	inline const char *name() {return #name;}

CRYPtcashP_DEFINE_NAME_STRING(ValueNames)			///< string, a list of value names with a semicolon (';') after each name
CRYPtcashP_DEFINE_NAME_STRING(Version)			///< int
CRYPtcashP_DEFINE_NAME_STRING(Seed)				///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(Key)				///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(IV)					///< ConstByteArrayParameter, also accepts const byte * for backwards compatibility
CRYPtcashP_DEFINE_NAME_STRING(StolenIV)			///< byte *
CRYPtcashP_DEFINE_NAME_STRING(Nonce)				///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(Rounds)				///< int
CRYPtcashP_DEFINE_NAME_STRING(FeedbackSize)		///< int
CRYPtcashP_DEFINE_NAME_STRING(WordSize)			///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(BlockSize)			///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(EffectiveKeyLength)	///< int, in bits
CRYPtcashP_DEFINE_NAME_STRING(KeySize)			///< int, in bits
CRYPtcashP_DEFINE_NAME_STRING(ModulusSize)		///< int, in bits
CRYPtcashP_DEFINE_NAME_STRING(SubgroupOrderSize)	///< int, in bits
CRYPtcashP_DEFINE_NAME_STRING(PrivateExponentSize)///< int, in bits
CRYPtcashP_DEFINE_NAME_STRING(Modulus)			///< Integer
CRYPtcashP_DEFINE_NAME_STRING(PublicExponent)		///< Integer
CRYPtcashP_DEFINE_NAME_STRING(PrivateExponent)	///< Integer
CRYPtcashP_DEFINE_NAME_STRING(PublicElement)		///< Integer
CRYPtcashP_DEFINE_NAME_STRING(SubgroupOrder)		///< Integer
CRYPtcashP_DEFINE_NAME_STRING(Cofactor)			///< Integer
CRYPtcashP_DEFINE_NAME_STRING(SubgroupGenerator)	///< Integer, ECP::Point, or EC2N::Point
CRYPtcashP_DEFINE_NAME_STRING(Curve)				///< ECP or EC2N
CRYPtcashP_DEFINE_NAME_STRING(GroupOID)			///< OID
CRYPtcashP_DEFINE_NAME_STRING(PointertcashrimeSelector)		///< const PrimeSelector *
CRYPtcashP_DEFINE_NAME_STRING(Prime1)				///< Integer
CRYPtcashP_DEFINE_NAME_STRING(Prime2)				///< Integer
CRYPtcashP_DEFINE_NAME_STRING(ModPrime1PrivateExponent)	///< Integer
CRYPtcashP_DEFINE_NAME_STRING(ModPrime2PrivateExponent)	///< Integer
CRYPtcashP_DEFINE_NAME_STRING(MultiplicativeInverseOfPrime2ModPrime1)	///< Integer
CRYPtcashP_DEFINE_NAME_STRING(QuadraticResidueModPrime1)	///< Integer
CRYPtcashP_DEFINE_NAME_STRING(QuadraticResidueModPrime2)	///< Integer
CRYPtcashP_DEFINE_NAME_STRING(PutMessage)			///< bool
CRYPtcashP_DEFINE_NAME_STRING(TruncatedDigestSize)	///< int
CRYPtcashP_DEFINE_NAME_STRING(BlockPaddingScheme)	///< StreamTransformationFilter::BlockPaddingScheme
CRYPtcashP_DEFINE_NAME_STRING(HashVerificationFilterFlags)		///< word32
CRYPtcashP_DEFINE_NAME_STRING(AuthenticatedDecryptionFilterFlags)	///< word32
CRYPtcashP_DEFINE_NAME_STRING(SignatureVerificationFilterFlags)	///< word32
CRYPtcashP_DEFINE_NAME_STRING(InputBuffer)		///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(OutputBuffer)		///< ByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(InputFileName)		///< const char *
CRYPtcashP_DEFINE_NAME_STRING(InputFileNameWide)	///< const wchar_t *
CRYPtcashP_DEFINE_NAME_STRING(InputStreamPointer)	///< std::istream *
CRYPtcashP_DEFINE_NAME_STRING(InputBinaryMode)	///< bool
CRYPtcashP_DEFINE_NAME_STRING(OutputFileName)		///< const char *
CRYPtcashP_DEFINE_NAME_STRING(OutputFileNameWide)	///< const wchar_t *
CRYPtcashP_DEFINE_NAME_STRING(OutputStreamPointer)	///< std::ostream *
CRYPtcashP_DEFINE_NAME_STRING(OutputBinaryMode)	///< bool
CRYPtcashP_DEFINE_NAME_STRING(EncodingParameters)	///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(KeyDerivationParameters)	///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(Separator)			///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(Terminator)			///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(Uppercase)			///< bool
CRYPtcashP_DEFINE_NAME_STRING(GroupSize)			///< int
CRYPtcashP_DEFINE_NAME_STRING(Pad)				///< bool
CRYPtcashP_DEFINE_NAME_STRING(PaddingByte)		///< byte
CRYPtcashP_DEFINE_NAME_STRING(Log2Base)			///< int
CRYPtcashP_DEFINE_NAME_STRING(EncodingLookupArray)	///< const byte *
CRYPtcashP_DEFINE_NAME_STRING(DecodingLookupArray)	///< const byte *
CRYPtcashP_DEFINE_NAME_STRING(InsertLineBreaks)	///< bool
CRYPtcashP_DEFINE_NAME_STRING(MaxLineLength)		///< int
CRYPtcashP_DEFINE_NAME_STRING(DigestSize)			///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(L1KeyLength)		///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(TableSize)			///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(Blinding)			///< bool, timing attack mitigations, ON by default
CRYPtcashP_DEFINE_NAME_STRING(DerivedKey)			///< ByteArrayParameter, key derivation, derived key
CRYPtcashP_DEFINE_NAME_STRING(DerivedKeyLength)	///< int, key derivation, derived key length in bytes
CRYPtcashP_DEFINE_NAME_STRING(Personalization)	///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(PersonalizationSize)	///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(Salt)				///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(Tweak)				///< ConstByteArrayParameter
CRYPtcashP_DEFINE_NAME_STRING(SaltSize)			///< int, in bytes
CRYPtcashP_DEFINE_NAME_STRING(TreeMode)			///< byte
CRYPtcashP_DEFINE_NAME_STRING(FileName)			///< const char *
CRYPtcashP_DEFINE_NAME_STRING(FileTime)			///< int
CRYPtcashP_DEFINE_NAME_STRING(Comment)			///< const char *
CRYPtcashP_DEFINE_NAME_STRING(Identity)			///< ConstByteArrayParameter
DOCUMENTED_NAMESPACE_END

NAMESPACE_END

#endif
